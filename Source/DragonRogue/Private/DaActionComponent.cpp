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
		AddAction(GetOwner(), ActionClass);
	}
}


void UDaActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FString DebugMsg = GetNameSafe(GetOwner()) + " " + ActiveGameplayTags.ToStringSimple();
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, DebugMsg);
}

void UDaActionComponent::AddAction(AActor* Instigator, TSubclassOf<UDaAction> ActionClass)
{
	if (!ensure(ActionClass))
	{
		return;
	}

	UDaAction* NewAction = NewObject<UDaAction>(this, ActionClass);
	if (ensure(NewAction))
	{
		Actions.Add(NewAction);

		if (NewAction->bAutoStart && ensure(NewAction->CanStart(Instigator)))
		{
			NewAction->StartAction(Instigator);
		}
	}
}

void UDaActionComponent::RemoveAction(UDaAction* ActionToRemove)
{
	if (!ensure(ActionToRemove && !ActionToRemove->IsRunning()))
	{
		return;
	}
	
	Actions.Remove(ActionToRemove);
}

bool UDaActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	for (UDaAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			if (!Action->CanStart(Instigator))
			{
				FString DebugMsg = FString::Printf(TEXT("Failed to run: %s"), *ActionName.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, DebugMsg);
				
				continue;
			}
			
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
			if (Action->IsRunning())
			{
				Action->StopAction(Instigator);
				return true;
			}
		}
	}
	return false;
}
