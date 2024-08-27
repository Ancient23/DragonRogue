// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DaPickupItem.h"
#include "GameplayTagContainer.h"
#include "DaActionPickupItem.generated.h"

class UDaAction;
/**
 * 
 */
UCLASS()
class DRAGONROGUE_API ADaActionPickupItem : public ADaPickupItem
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Actions")
	FGameplayTag ActionName;
	
	UPROPERTY(EditAnywhere, Category="Actions")
	TSubclassOf<UDaAction> ActionClass;

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	virtual FText GetInteractText_Implementation(APawn* InstigatorPawn) override;
};
