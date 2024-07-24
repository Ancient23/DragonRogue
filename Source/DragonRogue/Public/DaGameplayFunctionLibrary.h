// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DaGameplayFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONROGUE_API UDaGameplayFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Gameplay")
	static bool ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount);

	// HitResult passed as const reference becomes output pin in blueprints. If we passed just as Reference it would become an input pin
	UFUNCTION(BlueprintCallable, Category="Gameplay")
	static bool ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount, const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable, Category="Gameplay")
	static bool ApplyHealing(AActor* Healer, AActor* TargetActor, float HealingAmount);

	UFUNCTION(BlueprintCallable, Category="Gameplay")
	static bool ApplyHealingMax(AActor* Healer, AActor* TargetActor);
};
