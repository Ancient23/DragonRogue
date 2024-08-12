// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DaPlayerState.generated.h"

class UDaSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditsChanged, AActor*, InstigatorActor, int32, NewCreditAmount,
                                               int32, Delta);

/**
 * 
 */
UCLASS()
class DRAGONROGUE_API ADaPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	
	ADaPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:

	UPROPERTY(ReplicatedUsing="OnRep_Credits", EditDefaultsOnly, BlueprintReadOnly, Category="Credits")
	int32 Credits;

	UFUNCTION()
	void OnRep_Credits(int32 OldCredits);
	
public:
	
	UFUNCTION(BlueprintCallable, Category="Credits")
	int32 GetCredits() const;

	UFUNCTION(BlueprintCallable, Category="Credits")
	void AdjustCredits(int32 Delta);

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnCreditsChanged OnCreditsChanged;

	UFUNCTION(BlueprintNativeEvent)
	void SavePlayerState(UDaSaveGame* SaveObject);

	UFUNCTION(BlueprintNativeEvent)
	void LoadPlayerState(UDaSaveGame* SaveObject);
	
};
