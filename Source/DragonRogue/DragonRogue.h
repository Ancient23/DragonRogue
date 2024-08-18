// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_STATS_GROUP(TEXT("DREAMAWAKE_Game"), STATGROUP_DREAMAWAKE, STATCAT_Advanced);

// custom log category
DECLARE_LOG_CATEGORY_EXTERN(DragonRogue, Log, All);

// custom log macro
#define LOG(x, ...) UE_LOG(DragonRogue, Log, TEXT(x), ##__VA_ARGS__)
#define LOG_WARNING(x, ...) UE_LOG(DragonRogue, Warning, TEXT(x), ##__VA_ARGS__)
#define LOG_ERROR(x, ...) UE_LOG(DragonRogue, Error, TEXT(x), ##__VA_ARGS__)

static void LogOnScreen(UObject* WorldContext, FString Msg, FColor Color = FColor::White, float Duration = 5.0f)
{
	if (!ensure(WorldContext))
	{
		return;
	}

	UWorld* World = WorldContext->GetWorld();
	if (!ensure(World))
	{
		return;
	}

	FString NetPrefix = World->IsNetMode(NM_Client) ? "[CLIENT] " : "[SERVER] ";
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, NetPrefix + Msg);
	}
}

