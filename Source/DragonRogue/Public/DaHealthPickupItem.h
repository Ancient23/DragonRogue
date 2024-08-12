// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DaPickupItem.h"
#include "DaHealthPickupItem.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONROGUE_API ADaHealthPickupItem : public ADaPickupItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADaHealthPickupItem();
	
	UPROPERTY(EditAnywhere, Category="Health")
	float HealthAmount;

	UPROPERTY(EditAnywhere, Category="Credits")
	int32 Cost;
	
	void Interact_Implementation(APawn* InstigatorPawn) override;
};
