// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "DaBTTask_RangedAttack.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONROGUE_API UDaBTTask_RangedAttack : public UBTTaskNode
{
	GENERATED_BODY()

	/** starts this task, should return Succeeded, Failed or InProgress
	 *  (use FinishLatentTask() when returning InProgress)
	 * this function should be considered as const (don't modify state of object) if node is not instanced! */
	 virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	
	UPROPERTY(EditAnywhere, Category="AI")
	float MaxBulletSpread;

	UPROPERTY(EditAnywhere, Category="AI")
	TSubclassOf<AActor> ProjectileClass;

public:

	UDaBTTask_RangedAttack();
	
};
