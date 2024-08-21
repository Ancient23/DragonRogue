// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DaSaveGame.generated.h"

USTRUCT()
struct FActorSaveData
{
	GENERATED_BODY()

public:

	// ID of Actor
	UPROPERTY()
	FName ActorName;

	// For Movable Actors
	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	TArray<uint8> ByteData;
};

USTRUCT()
struct FPlayerSaveData
{
	GENERATED_BODY()

public:

	/* Player Id defined by the online sub system (such as Steam) converted to FString for simplicity  */ 
	UPROPERTY()
	int32 PlayerID;

	UPROPERTY()
	int32 Credits;

	/* Longest survival time */
	UPROPERTY()
	float PersonalRecordTime;

	/* Location if player was alive during save */
	UPROPERTY()
	FVector Location;

	/* Orientation if player was alive during save */ 
	UPROPERTY()
	FRotator Rotation;

	/* We don't always want to restore location, and may just resume player at specific respawn point in world. */
	UPROPERTY()
	bool bResumeAtTransform;
};

/**
 * 
 */
UCLASS()
class DRAGONROGUE_API UDaSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TArray<FPlayerSaveData> SavedPlayers;

	UPROPERTY()
	TArray<FActorSaveData> SavedActors;

	FPlayerSaveData* GetPlayerData(APlayerState* PlayerState);

};
