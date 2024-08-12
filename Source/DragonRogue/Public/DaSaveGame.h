// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DaSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONROGUE_API UDaSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY()
	int32 Credits;
	
};
