// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DaAction.h"
#include "DaActionEffect.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONROGUE_API UDaActionEffect : public UDaAction
{
	GENERATED_BODY()

public:

	void StartAction_Implementation(AActor* Instigator) override;

	void StopAction_Implementation(AActor* Instigator) override;

	UDaActionEffect();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects")
	float Duration;

	// Time in between ticks to apply effects
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects")
	float Period;

	FTimerHandle PeriodHandle;
	FTimerHandle DurationHandle;

	UFUNCTION(BlueprintNativeEvent, Category="Effect")
	void ExecutePeriodicEffect(AActor* Instigator);

};
