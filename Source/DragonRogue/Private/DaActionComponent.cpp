// Fill out your copyright notice in the Description page of Project Settings.


#include "DaActionComponent.h"

#include "DaAction.h"


UDaActionComponent::UDaActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UDaActionComponent::BeginPlay()
{
	Super::BeginPlay();

	for (TSubclassOf<UDaAction> ActionClass : DefaultActions)
	{
		AddAction(ActionClass);
	}
}


void UDaActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FString DebugMsg = GetNameSafe(GetOwner()) + " " + ActiveGameplayTags.ToStringSimple();
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, DebugMsg);
}

void UDaActionComponent::AddAction(TSubclassOf<UDaAction> ActionClass)
{
	if (!ensure(ActionClass))
	{
		return;
	}

	UDaAction* NewAction = NewObject<UDaAction>(this, ActionClass);
	if (ensure(NewAction))
	{
		Actions.Add(NewAction);
	}
}

bool UDaActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	for (UDaAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			Action->StartAction(Instigator);
			return true;
		}
	}
	return false;
}

bool UDaActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for (UDaAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			Action->StopAction(Instigator);
			return true;
		}
	}
	return false;
}
