// Fill out your copyright notice in the Description page of Project Settings.


#include "DaGameModeBase.h"

#include "DaActionComponent.h"
#include "DaAttributeComponent.h"
#include "DaCharacter.h"
#include "DaPickupItem.h"
#include "DaPlayerState.h"
#include "AI/DaAICharacter.h"
#include "DragonRogue/DragonRogue.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "EngineUtils.h"
#include "DragonRogue/DaConstants.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "DaMonsterData.h"
#include "DaSaveGameSubsystem.h"
#include "Engine/AssetManager.h"

static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("da.SpawnBots"), true, TEXT("Enable Spawning of Bots with Timer"), ECVF_Cheat);
static TAutoConsoleVariable<bool> CVarDebugSpawnBots(TEXT("da.DrawDebugSpawnBots"), false, TEXT("Draw Debug Spheres showing location where Bots spawned"), ECVF_Cheat);
static TAutoConsoleVariable<bool> CVarDebugSpawnItems(TEXT("da.DrawDebugSpawnItems"), false, TEXT("Draw Debug Spheres showing location where Items spawned"), ECVF_Cheat);

ADaGameModeBase::ADaGameModeBase()
{
	SpawnTimerInterval=2.0f;
	CreditsPerKill = 10;
	MaxBotsOverride = 0;

	bAutoRespawnPlayer = true;
}

void ADaGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	UDaSaveGameSubsystem* SG = GetGameInstance<UDaSaveGameSubsystem>();
	
	FString SelectedSaveSlot = UGameplayStatics::ParseOption(Options, "SaveGame");
	SG->LoadSaveGame(SelectedSaveSlot);
}

void ADaGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ADaGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);

	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnItemQuery, this, EEnvQueryRunMode::AllMatching, nullptr);
	if (ensure(QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ADaGameModeBase::OnSpawnItemQueryCompleted);
	}
}

void ADaGameModeBase::OnSpawnItemQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
	EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		LOG_WARNING("Spawn ITEM EQS Query Failed");
		return;
	}

	TArray<FGameplayTag> ItemTags;
	int32 TagCount = ItemClasses.GetKeys(ItemTags);
	if (TagCount==0){
		LOG_WARNING("Spawn ITEM Failed, Tags and Item classes not Defined in Gamemode");
		return;
	}
	
	int32 PickupTypes = ItemTags.Num();
	int32 CurrentType = 0;
	if (PickupTypes==0)
	{
		LOG_WARNING("Spawn ITEM Failed, Item classes not Defined in Gamemode");
		return;
	}

	//@TODO: Currently relying on EQS to filter out items, but we could just a get a grid and use something like "Algo/RandomShuffle.h" and pick the first X items from the array
	// Also could introduce a spawn actor class that can be dropped in the map, instead of relying on GameMode's location (the QueryContext)
	
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	for (FVector Loc : Locations)
	{
		FGameplayTag CurrentTag = ItemTags[CurrentType++];
		if (CurrentType == PickupTypes)
		{
			CurrentType=0;
		}
		
		if (!CurrentTag.MatchesTag(TAG_PICKUP))
		{
			LOG_WARNING("Non Matching Pickup GameplayTag: %s", *CurrentTag.ToString());
			continue;
		}
		
		// Raise above ground level
		Loc.Z += 100.0f;
		GetWorld()->SpawnActor<ADaPickupItem>(ItemClasses[CurrentTag], Loc, FRotator::ZeroRotator);
		
		// Debug
		if (CVarDebugSpawnItems.GetValueOnGameThread())
		{
			DrawDebugSphere(GetWorld(), Loc, 50.0f, 20, FColor::Yellow, false, 60.f);
			FString DebugMsg = FString::Printf(TEXT("Spawned %s at Location: %s"), *CurrentTag.ToString(), *Loc.ToString());
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, DebugMsg);
		}
	}
}

void ADaGameModeBase::SpawnBotTimerElapsed()
{
	if (!CVarSpawnBots.GetValueOnGameThread())
	{
		LOG_WARNING("Bot Spawning Disabled via CVar 'CVarSpawnBots'.");
		return;
	}
	
	int32 NumberOfAliveBots = 0;
	for (TActorIterator<ADaAICharacter> It(GetWorld()); It; ++It)
	{
		ADaAICharacter* Bot = *It;

		UDaAttributeComponent* AttribComp = UDaAttributeComponent::GetAttributes(Bot);
		if (ensure(AttribComp) && AttribComp->IsAlive())
		{
			NumberOfAliveBots++;
		}
	}

	LOG("Found %i alive bots", NumberOfAliveBots);
	
	float MaxBotCount = MaxBotsOverride;
	if (MaxBotsOverride == 0 && DifficultyCurve)
	{
		MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}

	if (NumberOfAliveBots >= MaxBotCount)
	{
		LOG("At Max Capacity, %i alive bots. Skipping spawn", NumberOfAliveBots);
		return;
	}
	
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);
	if (ensure(QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ADaGameModeBase::OnSpawnBotQueryCompleted);
	}
}

void ADaGameModeBase::OnSpawnBotQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
	EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		//UE_LOG(DragonRogue, Warning, TEXT("Spawn bot EQS Query Failed"));
		LOG_WARNING("Spawn bot EQS Query Failed");
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	if (Locations.IsValidIndex(0) && MonsterTable)
	{
		LogOnScreen(this, "Begin Loading", FColor::Yellow);

		TArray<FMonsterInfoRow*> Rows;
		MonsterTable->GetAllRows("", Rows);

		//Get Random Enemy
		int32 RandomIndex = FMath::RandRange(0, Rows.Num() - 1);
		FMonsterInfoRow* SelectedRow = Rows[RandomIndex];

		UAssetManager& AssMan = UAssetManager::Get();

		TArray<FName> Bundles; 
		FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ADaGameModeBase::OnMonsterLoaded, SelectedRow->MonsterId, Locations[0]);
		AssMan.LoadPrimaryAsset(SelectedRow->MonsterId, Bundles, Delegate);
	}
}

void ADaGameModeBase::OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation)
{
	LogOnScreen(this, "Finished Loading", FColor::Green);
	
	UAssetManager& AssMan = UAssetManager::Get();

	UDaMonsterData* MonsterData = Cast<UDaMonsterData>(AssMan.GetPrimaryAssetObject(LoadedId));
	if (MonsterData)
	{
		AActor* Monster = GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator);
		if (Monster)
		{
			LogOnScreen(this, FString::Printf(TEXT("Spawned Enemy: %s (%s)"), *GetNameSafe(Monster), *GetNameSafe(MonsterData)));
			UDaActionComponent* ActionComp = Cast<UDaActionComponent>(Monster->GetComponentByClass(UDaActionComponent::StaticClass()));
			if (ActionComp)
			{
				for (TSubclassOf<UDaAction> ActionClass : MonsterData->Actions)
				{
					ActionComp->AddAction(Monster, ActionClass);
				}
			}
		}
	}
	
	if (CVarDebugSpawnBots.GetValueOnGameThread())
	{
		DrawDebugSphere(GetWorld(), SpawnLocation, 50.0f, 20, FColor::Blue, false, 60.f);
	}
}

void ADaGameModeBase::KillAll()
{
	for (TActorIterator<ADaAICharacter> It(GetWorld()); It; ++It)
	{
		ADaAICharacter* Bot = *It;

		UDaAttributeComponent* AttribComp = UDaAttributeComponent::GetAttributes(Bot);
		if (ensure(AttribComp) && AttribComp->IsAlive())
		{
			AttribComp->Kill(this); //@TODO: pass in player for kill credit?
		}
	}
}

void ADaGameModeBase::OnActorKilled(AActor* VictimActor, AActor* KillerActor)
{
	// player dies -> timer elapsed, respawn player
	ADaCharacter* Player = Cast<ADaCharacter>(VictimActor);
	if (Player)
	{
		if (bAutoRespawnPlayer)
		{
			FTimerHandle TimerHandle_RespawnDelay;
			float RespondDelay = 2.0f;
			FTimerDelegate Delegate;
			Delegate.BindUObject(this, &ThisClass::RespawnPlayerElapsed, Player->GetController());
			GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespondDelay, false);
		}
		
		// Store time if it was better than previous record
		ADaPlayerState* PS = Player->GetPlayerState<ADaPlayerState>();
		if (PS)
		{
			PS->UpdatePersonalRecord(GetWorld()->TimeSeconds);
		}
		
		// Immediatly AutoSave on Player Death
		UDaSaveGameSubsystem* SG = GetGameInstance<UDaSaveGameSubsystem>();
		SG->WriteSaveGame();
	}
	else if (ADaCharacter* PlayerActor = Cast<ADaCharacter>(KillerActor))
	{
		// AI Character was killed. Grant Credits to player
		ADaPlayerState* PlayerState = Cast<ADaPlayerState>(PlayerActor->GetPlayerState());
		ensureAlways(PlayerState);

		// @FIXME: Crash when action effect kills AI, but Player was killed first. 
		if (PlayerState)
		{
			PlayerState->AdjustCredits(CreditsPerKill);
		}
	}

	//LOG("OnActorKilled: Victim: %s, Killer: %s", *GetNameSafe(VictimActor), *GetNameSafe(KillerActor));
	//FString Msg = FString::Printf(TEXT("OnActorKilled: Victim: %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(KillerActor));
	//LogOnScreen(this, Msg, FColor::Orange);
}

void ADaGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if (ensure(Controller))
	{
		//detach
		Controller->UnPossess();
		
		RestartPlayer(Controller);
	}
}

void ADaGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{

	UDaSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<UDaSaveGameSubsystem>();
	SG->HandleStartingNewPlayer(NewPlayer);

	// Will call BeginPlaying State in Player Controller so make sure our data is setup before this calls super
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	// Override spawn location
	SG->OverrideSpawnTransform(NewPlayer);
}
