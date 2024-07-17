// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// custom log category
DECLARE_LOG_CATEGORY_EXTERN(DragonRogue, Log, All);

// custom log macro
#define LOG(x, ...) UE_LOG(DragonRogue, Log, TEXT(x), ##__VA_ARGS__)
#define LOG_WARNING(x, ...) UE_LOG(DragonRogue, Warning, TEXT(x), ##__VA_ARGS__)
#define LOG_ERROR(x, ...) UE_LOG(DragonRogue, Error, TEXT(x), ##__VA_ARGS__)