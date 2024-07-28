// Fill out your copyright notice in the Description page of Project Settings.


#include "DaAction.h"

#include "DaActionComponent.h"
#include "DragonRogue/DragonRogue.h"

void UDaAction::StartAction_Implementation(AActor* Instigator)
{
	LOG("Running %s", *GetNameSafe(this));

	UDaActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.AppendTags(GrantTags);
}

void UDaAction::StopAction_Implementation(AActor* Instigator)
{
	LOG("Stopped %s", *GetNameSafe(this));

	UDaActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.RemoveTags(GrantTags);
}

UDaActionComponent* UDaAction::GetOwningComponent() const
{
	return Cast<UDaActionComponent>(GetOuter());
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
