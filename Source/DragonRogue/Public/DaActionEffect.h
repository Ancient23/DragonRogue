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

	virtual void StartAction_Implementation(AActor* Instigator) override;

	virtual void StopAction_Implementation(AActor* Instigator) override;

	UFUNCTION(BlueprintCallable, Category="Action")
	float GetTimeRemaining() const;
	
	UDaActionEffect();

protected:

	// NOTE: Duration == 0 will be infinite. Stop Action wont get called.
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
