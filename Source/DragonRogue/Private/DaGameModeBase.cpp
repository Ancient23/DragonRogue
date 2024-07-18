// Fill out your copyright notice in the Description page of Project Settings.


#include "DaGameModeBase.h"

#include "DaAttributeComponent.h"
#include "AI/DaAICharacter.h"
#include "DragonRogue/DragonRogue.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "EngineUtils.h"

ADaGameModeBase::ADaGameModeBase()
{
	SpawnTimerInterval=2.0f;
}

void ADaGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ADaGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
	
}

void ADaGameModeBase::SpawnBotTimerElapsed()
{
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
	
	float MaxBotCount = 10.f;
	if (DifficultyCurve)
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
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ADaGameModeBase::OnQueryCompleted);
	}
}

void ADaGameModeBase::OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
	EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		//UE_LOG(DragonRogue, Warning, TEXT("Spawn bot EQS Query Failed"));
		LOG_WARNING("Spawn bot EQS Query Failed");
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	if (Locations.IsValidIndex(0))
	{
		GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);

		DrawDebugSphere(GetWorld(), Locations[0], 50.0f, 20, FColor::Blue, false, 60.f);
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