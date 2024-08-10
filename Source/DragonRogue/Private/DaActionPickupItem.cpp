// Fill out your copyright notice in the Description page of Project Settings.


#include "DaActionPickupItem.h"
#include "DaActionComponent.h"

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
