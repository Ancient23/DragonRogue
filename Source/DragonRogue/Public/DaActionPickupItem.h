// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DaPickupItem.h"
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
	FName ActionName;
	
	UPROPERTY(EditAnywhere, Category="Actions")
	TSubclassOf<UDaAction> ActionClass;

	void Interact_Implementation(APawn* InstigatorPawn) override;
};
