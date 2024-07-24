// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/DaBTTask_HealToMax.h"

#include "AIController.h"
#include "DaGameplayFunctionLibrary.h"

UDaBTTask_HealToMax::UDaBTTask_HealToMax()
{
	HealIntervalAmount = 10.0f;
	HealIntervalDelay = 3.0f;
}

EBTNodeResult::Type UDaBTTask_HealToMax::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (ensure(MyController))
	{
		APawn* Pawn = MyController->GetPawn();
		if (Pawn == nullptr)
		{
			return EBTNodeResult::Failed;
		}
		
		// Heal Max for now
		//@TODO: Look into the Tick features of BTTask to heal slowly over an interval.
		bool bHealed = UDaGameplayFunctionLibrary::ApplyHealingMax(Pawn, Pawn);

		return bHealed ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}
	
	return EBTNodeResult::Failed;
}

