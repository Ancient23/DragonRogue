// Fill out your copyright notice in the Description page of Project Settings.


#include "DaSaveGameSubsystem.h"

#include "DaGameplayInterface.h"
#include "DaPlayerState.h"
#include "DaSaveGame.h"
#include "DaSaveGameSettings.h"
#include "EngineUtils.h"
#include "DragonRogue/DragonRogue.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

void UDaSaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UDaSaveGameSettings* SGSettings = GetDefault<UDaSaveGameSettings>();
	// Access defaults from DefaultGame.ini
	CurrentSlotName = SGSettings->SaveSlotName;

	// Make sure its loaded into memeory. .Get() Only resolves if already loaded previously elsewhere in code
	UDataTable* DummyTable = SGSettings->DummyTablePath.LoadSynchronous();
	// Dummytable->GetAllRows() // not needed currently, just an example
}

void UDaSaveGameSubsystem::HandleStartingNewPlayer(AController* NewPlayer)
{
	ADaPlayerState* PS = NewPlayer->GetPlayerState<ADaPlayerState>();
	if (ensure(PS))
	{
		PS->LoadPlayerState(CurrentSaveGame);
	}
}

bool UDaSaveGameSubsystem::OverrideSpawnTransform(AController* NewPlayer)
{
	if (NewPlayer == nullptr || NewPlayer->IsPendingKillPending())
	{
		return false;
	}

	APlayerState* PS = NewPlayer->GetPlayerState<APlayerState>();
	if (PS == nullptr)
	{
		return false;
	}

	if (APawn* MyPawn = PS->GetPawn())
	{
		FPlayerSaveData* FoundData = CurrentSaveGame->GetPlayerData(PS);
		if (FoundData && FoundData->bResumeAtTransform)
		{		
			MyPawn->SetActorLocation(FoundData->Location);
			MyPawn->SetActorRotation(FoundData->Rotation);

			// PlayerState owner is a (Player)Controller
			AController* PC = Cast<AController>(PS->GetOwner());
			// Set control rotation to change camera direction, setting Pawn rotation is not enough
			PC->SetControlRotation(FoundData->Rotation);

			return true;
		}
	}

	return false;
}

void UDaSaveGameSubsystem::WriteSaveGame()
{
	// Clear arrays, may contain data from previously loaded SaveGame
	CurrentSaveGame->SavedPlayers.Empty();
	CurrentSaveGame->SavedActors.Empty();

	AGameStateBase* GS = GetWorld()->GetGameState();
	if (GS == nullptr)
	{
		// Warn about failure to save?
		return;
	}

	// Iterate all player states, we don't have proper ID to match yet (requires Steam or EOS)
	for (int32 i = 0; i < GS->PlayerArray.Num(); i++)
	{
		ADaPlayerState* PS = Cast<ADaPlayerState>(GS->PlayerArray[i]);
		if (PS)
		{
			PS->SavePlayerState(CurrentSaveGame);
			break; // single player only at this point
		}
	}

	// Iterate the entire world of actors
	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		// Only interested in our 'gameplay actors', skip actors that are being destroyed
		// Note: You might instead use a dedicated SavableObject interface for Actors you want to save instead of re-using GameplayInterface
		if (Actor->IsPendingKillPending() || !Actor->Implements<UDaGameplayInterface>())
		{
			continue;
		}

		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetFName();
		ActorData.Transform = Actor->GetActorTransform();

		// Pass the array to fill with data from Actor
		FMemoryWriter MemWriter(ActorData.ByteData);

		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
		// Find only variables with UPROPERTY(SaveGame)
		Ar.ArIsSaveGame = true;
		// Converts Actor's SaveGame UPROPERTIES into binary array
		Actor->Serialize(Ar);

		CurrentSaveGame->SavedActors.Add(ActorData);
	}

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, CurrentSlotName, 0);

	OnSaveGameWritten.Broadcast(CurrentSaveGame);
}

void UDaSaveGameSubsystem::LoadSaveGame(FString InSlotName)
{
	// Update slot name first if specified, otherwise keeps default name
	SetSlotName(InSlotName);

	if (UGameplayStatics::DoesSaveGameExist(CurrentSlotName, 0))
	{
		CurrentSaveGame = Cast<UDaSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSlotName, 0));
		if (CurrentSaveGame == nullptr)
		{
			LOG("Failed to load SaveGame Data.");
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("Loaded SaveGame Data."));


		// Iterate the entire world of actors
		for (FActorIterator It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;
			// Only interested in our 'gameplay actors'
			if (!Actor->Implements<UDaGameplayInterface>())
			{
				continue;
			}

			for (FActorSaveData ActorData : CurrentSaveGame->SavedActors)
			{
				if (ActorData.ActorName == Actor->GetFName())
				{
					Actor->SetActorTransform(ActorData.Transform);

					FMemoryReader MemReader(ActorData.ByteData);

					FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
					Ar.ArIsSaveGame = true;
					// Convert binary array back into actor's variables
					Actor->Serialize(Ar);

					IDaGameplayInterface::Execute_OnActorLoaded(Actor);

					break;
				}
			}
		}

		OnSaveGameLoaded.Broadcast(CurrentSaveGame);
	}
	else
	{
		CurrentSaveGame = Cast<UDaSaveGame>(UGameplayStatics::CreateSaveGameObject(UDaSaveGame::StaticClass()));

		LOG("Created New SaveGame Data.");
	}
}

void UDaSaveGameSubsystem::SetSlotName(FString NewSlotName)
{
	// Ignore empty name
	if (NewSlotName.Len() == 0)
	{
		return;
	}

	CurrentSlotName = NewSlotName;
}
