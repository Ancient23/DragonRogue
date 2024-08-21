// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DaPlayerState.generated.h"

class UDaSaveGame;
class ADaPlayerState; // for macros

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditsChanged, AActor*, InstigatorActor, int32, NewCreditAmount,
                                               int32, Delta);
// Event Handler for Personal Record Time
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRecordTimeChanged, ADaPlayerState*, PlayerState, float, NewTime, float, OldRecord);

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

	UPROPERTY(BlueprintReadOnly)
	float PersonalRecordTime;
	
	UFUNCTION()
	void OnRep_Credits(int32 OldCredits);
	
public:
	
	UFUNCTION(BlueprintCallable, Category="Credits")
	int32 GetCredits() const;

	UFUNCTION(BlueprintCallable, Category="Credits")
	void AdjustCredits(int32 Delta);

	/* Checks current record and only sets if better time was passed in. */
	UFUNCTION(BlueprintCallable)
	bool UpdatePersonalRecord(float NewTime);
	
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnCreditsChanged OnCreditsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRecordTimeChanged OnRecordTimeChanged;
	
	UFUNCTION(BlueprintNativeEvent)
	void SavePlayerState(UDaSaveGame* SaveObject);

	UFUNCTION(BlueprintNativeEvent)
	void LoadPlayerState(UDaSaveGame* SaveObject);
	
};
