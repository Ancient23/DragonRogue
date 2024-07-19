// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "DaBTService_CheckLowHealth.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONROGUE_API UDaBTService_CheckLowHealth : public UBTService
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category="AI")
	FBlackboardKeySelector LowHealthKey;
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
