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
	LogOnScreen(this, FString::Printf(TEXT("Started %s"), *ActionName.ToString()), FColor::Green);

	UDaActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.AppendTags(GrantTags);

	bIsRunning = true;
}

void UDaAction::StopAction_Implementation(AActor* Instigator)
{
	LogOnScreen(this, FString::Printf(TEXT("Started %s"), *ActionName.ToString()), FColor::White);
	
	UDaActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.RemoveTags(GrantTags);

	bIsRunning = false;
}

void UDaAction::OnRep_IsRunning()
{
	if (bIsRunning)
	{
		StartAction(nullptr);
	} else
	{
		StopAction(nullptr);
	}
}

bool UDaAction::IsRunning() const
{
	return bIsRunning;
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

	DOREPLIFETIME(UDaAction, bIsRunning);
}

