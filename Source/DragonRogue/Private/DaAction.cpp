// Fill out your copyright notice in the Description page of Project Settings.


#include "DaAction.h"

#include "DaActionComponent.h"
#include "DragonRogue/DragonRogue.h"
#include "Net/UnrealNetwork.h"

bool UDaAction::CanStart_Implementation(AActor* Instigator)
{
	if (IsRunning())
	{
		return false;
	}
	
	UDaActionComponent* Comp = GetOwningComponent();
	if (Comp->ActiveGameplayTags.HasAny(BlockedTags))
	{
		return false;
	}

	return true;
}

void UDaAction::StartAction_Implementation(AActor* Instigator)
{
	//LogOnScreen(this, FString::Printf(TEXT("Started %s"), *ActionName.ToString()), FColor::Green);
	LOG("Started %s", *GetNameSafe(this));
	
	UDaActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.AppendTags(GrantTags);

	RepData.bIsRunning = true;
	RepData.Instigator = Instigator;

	if (GetOwningComponent()->GetOwnerRole() == ROLE_Authority)
	{
		TimeStarted = GetWorld()->TimeSeconds;
	}
	
	Comp->OnActionStarted.Broadcast(Comp, this);
}

void UDaAction::StopAction_Implementation(AActor* Instigator)
{
	//LogOnScreen(this, FString::Printf(TEXT("Started %s"), *ActionName.ToString()), FColor::White);
	LOG("Stopped %s", *GetNameSafe(this));
	
	UDaActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.RemoveTags(GrantTags);

	RepData.bIsRunning = false;

	Comp->OnActionStopped.Broadcast(Comp, this);
}

void UDaAction::OnRep_IsRunning()
{
	if (RepData.bIsRunning)
	{
		StartAction(RepData.Instigator);
	} else
	{
		StopAction(RepData.Instigator);
	}
}

bool UDaAction::IsRunning() const
{
	return RepData.bIsRunning;
}

UWorld* UDaAction::GetWorld() const
{
	UActorComponent* Comp = Cast<UActorComponent>(GetOuter());
	if (Comp)
	{
		return Comp->GetWorld();
	}

	return nullptr;
}

UDaActionComponent* UDaAction::GetOwningComponent() const
{
	return Cast<UDaActionComponent>(GetOuter());
}

void UDaAction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDaAction, RepData);
	DOREPLIFETIME(UDaAction, TimeStarted);

}

