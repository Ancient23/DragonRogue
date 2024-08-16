// Fill out your copyright notice in the Description page of Project Settings.


#include "DaActionPickupItem.h"
#include "DaActionComponent.h"

#define LOCTEXT_NAMESPACE "InteractableActors"

void ADaActionPickupItem::Interact_Implementation(APawn* InstigatorPawn)
{
	if (InstigatorPawn)
	{
		UDaActionComponent* ActionComp = UDaActionComponent::GetActions(InstigatorPawn);
		if (ActionComp && !ActionComp->ContainsActionWithName(ActionName))
		{
			ActionComp->AddAction(InstigatorPawn, ActionClass);

			HideAndCooldownItem(InstigatorPawn);
		}
	}
}

FText ADaActionPickupItem::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	return FText::Format(LOCTEXT("CreditsPickup_InteractMessage", "Receive {0} Ability"), FText::FromName(ActionName));
}

#undef LOCTEXT_NAMESPACE