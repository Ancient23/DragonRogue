// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/DaBTService_CheckLowHealth.h"
#include "AIController.h"
#include "DaAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"


void UDaBTService_CheckLowHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackboardComp))
	{
		AAIController* MyController = OwnerComp.GetAIOwner();
		if (ensure(MyController))
		{
			APawn* AIPawn = MyController->GetPawn();
			if (ensure(AIPawn))
			{
				// Check Attribute component to see if Health is below a certain threshold
				bool bHasLowHealth = UDaAttributeComponent::HasLowHealth(AIPawn);
				BlackboardComp->SetValueAsBool(LowHealthKey.SelectedKeyName, bHasLowHealth);
			}
		}
	}
}
