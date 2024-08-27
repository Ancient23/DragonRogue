// Fill out your copyright notice in the Description page of Project Settings.


#include "DaActionComponent.h"

#include "DaAction.h"
#include "DragonRogue/DragonRogue.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

DECLARE_CYCLE_STAT(TEXT("StartActionByName"), STAT_StartActionByName, STATGROUP_DREAMAWAKE);

UDaActionComponent::UDaActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void UDaActionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		for (TSubclassOf<UDaAction> ActionClass : DefaultActions)
		{
			AddAction(GetOwner(), ActionClass);
		}
	}
	bHasBegunPlay = true;
}

void UDaActionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Stop all - make a copy of array in case main array is being mutated
	TArray<UDaAction*> ActionsCopy = Actions;
	for (UDaAction* Action : ActionsCopy)
	{
		if (Action && Action->IsRunning())
		{
			Action->StopAction(GetOwner());
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

void UDaActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Debug Gameplay tags
	//FString DebugMsg = GetNameSafe(GetOwner()) + " " + ActiveGameplayTags.ToStringSimple();
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, DebugMsg);

	// TODO: DEBUG - ENABLE ONLY WITH CVAR
	// Draw All Actions
	for (UDaAction* Action : Actions)
	{
		FColor TextColor = Action->IsRunning() ? FColor::Blue : FColor::White;
		FString ActionMsg = FString::Printf(TEXT("[%s] Action: %s"), *GetNameSafe(GetOwner()), *GetNameSafe(Action));
		LogOnScreen(this, ActionMsg, TextColor, 0.0f);
	}
}

UDaActionComponent* UDaActionComponent::GetActions(AActor* FromActor)
{
	if (FromActor)
	{
		return Cast<UDaActionComponent>(FromActor->GetComponentByClass(UDaActionComponent::StaticClass()));
	}

	return nullptr;
}

void UDaActionComponent::AddAction(AActor* Instigator, TSubclassOf<UDaAction> ActionClass)
{
	if (!ensure(ActionClass))
	{
		return;
	}

	if (!GetOwner()->HasAuthority())
	{
		LOG_WARNING("Client Attempting to AddAction. [Class: %s]", *GetNameSafe(ActionClass));
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

bool UDaActionComponent::StartActionByName(AActor* Instigator, FGameplayTag ActionName)
{
	SCOPE_CYCLE_COUNTER(STAT_StartActionByName);
	
	for (UDaAction* Action : Actions)
	{
		if (Action && Action->ActivationTag == ActionName)
		{
			if (!Action->CanStart(Instigator))
			{
				FString DebugMsg = FString::Printf(TEXT("Failed to run: %s"), *ActionName.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, DebugMsg);
				
				continue;
			}

			// Is Client?
			if (!GetOwner()->HasAuthority())
			{
				ServerStartAction(Instigator, ActionName);
			}

			// Bookmark here to mark when this happens in Unreal Insights.
			TRACE_BOOKMARK(TEXT("StartAction::%s"), *GetNameSafe(Action));
			
			Action->StartAction(Instigator);
			return true;
		}
	}
	return false;
}

bool UDaActionComponent::StopActionByName(AActor* Instigator, FGameplayTag ActionName)
{
	for (UDaAction* Action : Actions)
	{
		if (Action && Action->ActivationTag == ActionName)
		{
			if (Action->IsRunning())
			{

				// Is Client?
				if (!GetOwner()->HasAuthority())
				{
					ServerStopAction(Instigator, ActionName);
				}
				
				Action->StopAction(Instigator);
				return true;
			}
		}
	}
	return false;
}

bool UDaActionComponent::ContainsActionWithName(FGameplayTag ActionName)
{
	for (UDaAction* Action : Actions)
	{
		if (Action && Action->ActivationTag == ActionName)
		{
			return true;
		}
	}

	return false;
}

float UDaActionComponent::GetActionCostByName(FGameplayTag ActionName)
{
	for (UDaAction* Action : Actions)
	{
		if (Action && Action->ActivationTag == ActionName)
		{
			return Action->CostToActivate;
		}
	}

	return 0.0f;
}

void UDaActionComponent::ServerStartAction_Implementation(AActor* Instigator, FGameplayTag ActionName)
{
	StartActionByName(Instigator, ActionName);
}

void UDaActionComponent::ServerStopAction_Implementation(AActor* Instigator, FGameplayTag ActionName)
{
	StopActionByName(Instigator, ActionName);
}

void UDaActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDaActionComponent, Actions);
	
}

bool UDaActionComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (UDaAction* Action : Actions)
	{
		if (Action)
		{
			WroteSomething |= Channel->ReplicateSubobject(Action, *Bunch, *RepFlags);
		}
	}
	
	return WroteSomething;
}
