// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/DaAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void ADaAIController::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(BehaviorTree, TEXT("Behavior Tree is nullptr! Please assign BehaviorTree in your AI Controller")))
	{
		RunBehaviorTree(BehaviorTree);
	}

	// APawn* Mypawn = UGameplayStatics::GetPlayerPawn(this, 0);
	// if (Mypawn)
	// {
	// 	GetBlackboardComponent()->SetValueAsVector("MoveToLocation", Mypawn->GetActorLocation());
	// 	
	// 	GetBlackboardComponent()->SetValueAsObject("TargetActor", Mypawn);
	//
	// }
}
