// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "DaSaveGameSettings.generated.h"

class UDataTable;
/**
 * 
 */
UCLASS(Config=Game, defaultconfig, meta = (DisplayName="Save Game Settings")) // 'defaultconfig' = "Save object config only to Default INIs, never to local INIs."
class DRAGONROGUE_API UDaSaveGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	
	/* Default slot name if UI doesn't specify any */ 
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	FString SaveSlotName;

	/* Soft path must be converted to asset before use */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General", AdvancedDisplay)
	TSoftObjectPtr<UDataTable> DummyTablePath;

	UDaSaveGameSettings();
};
