// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DaPickupItem.h"
#include "DaCreditsPickupItem.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONROGUE_API ADaCreditsPickupItem : public ADaPickupItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Credits")
	int32 CreditsAmount;
	
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	virtual FText GetInteractText_Implementation(APawn* InstigatorPawn) override;

	ADaCreditsPickupItem();
};
