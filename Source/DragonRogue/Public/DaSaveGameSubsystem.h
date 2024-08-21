// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DaSaveGameSubsystem.generated.h"

class UDaSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGameSignature, class UDaSaveGame*, SaveObject);

/**
 * 
 */
UCLASS(meta=(DisplayName="SaveGame System"))
class DRAGONROGUE_API UDaSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:

	// Name of Slot to Save/Load to disk. Filled by SaveGameSettings (can be overriden from GameMode's InitGame()
	FString CurrentSlotName;

	UPROPERTY()
	UDaSaveGame* CurrentSaveGame;

public:

	// Restore Serialized data from PlayerState into Player
	void HandleStartingNewPlayer(AController* NewPlayer);

	// Restore Spawn Transform using stored data per PlayerState after being fully initialized
	UFUNCTION(BlueprintCallable)
	bool OverrideSpawnTransform(AController* NewPlayer);

	// Change slot name, will be used next time we load/save data
	UFUNCTION(BlueprintCallable)
	void SetSlotName(FString NewSlotName);
	
	UFUNCTION(BlueprintCallable, Category="SaveGame")
	void WriteSaveGame();

	// Load from disk, optional slot name
	void LoadSaveGame(FString InSlotName="");

	UPROPERTY(BlueprintAssignable)
	FOnSaveGameSignature OnSaveGameLoaded;

	UPROPERTY(BlueprintAssignable)
	FOnSaveGameSignature OnSaveGameWritten;

	// initialize subsystem, good moment to load in SaveGameSettings variables
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};
