// Fill out your copyright notice in the Description page of Project Settings.


#include "DaAction.h"

#include "DaActionComponent.h"
#include "DragonRogue/DragonRogue.h"

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
	LOG("Running %s", *GetNameSafe(this));

	UDaActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.AppendTags(GrantTags);

	bIsRunning = true;
}

void UDaAction::StopAction_Implementation(AActor* Instigator)
{
	LOG("Stopped %s", *GetNameSafe(this));

	ensureAlways(bIsRunning);
	
	UDaActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.RemoveTags(GrantTags);

	bIsRunning = false;
}

UDaActionComponent* UDaAction::GetOwningComponent() const
{
	return Cast<UDaActionComponent>(GetOuter());
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
