// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DaPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditsChanged, AActor*, InstigatorActor, float, NewCreditAmount, float, Delta);

/**
 * 
 */
UCLASS()
class DRAGONROGUE_API ADaPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category="Credits")
	float Credits;
	
public:

	UFUNCTION(BlueprintCallable, Category="Credits")
	float GetCredits() const;

	UFUNCTION(BlueprintCallable, Category="Credits")
	void AdjustCredits(float Delta);

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnCreditsChanged OnCreditsChanged;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastCreditsChanged(AActor* InstigatorActor, float NewCreditAmount, float Delta);
	
	ADaPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
