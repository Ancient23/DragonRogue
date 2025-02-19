// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "DaBTTask_HealToMax.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONROGUE_API UDaBTTask_HealToMax : public UBTTaskNode
{
	GENERATED_BODY()
	
	/** starts this task, should return Succeeded, Failed or InProgress
	 *  (use FinishLatentTask() when returning InProgress)
	 * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	FTimerHandle TimerHandle_Healing;
	
	UPROPERTY(EditAnywhere, Category="AI")
	float HealIntervalAmount;

	UPROPERTY(EditAnywhere, Category="AI")
	float HealIntervalDelay;

public:

	UDaBTTask_HealToMax();
	
};
