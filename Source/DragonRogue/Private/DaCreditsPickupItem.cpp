// Fill out your copyright notice in the Description page of Project Settings.


#include "DaCreditsPickupItem.h"

#include "DaPlayerState.h"

#define LOCTEXT_NAMESPACE "InteractableActors"

ADaCreditsPickupItem::ADaCreditsPickupItem()
{
	CreditsAmount = 10;
}

void ADaCreditsPickupItem::Interact_Implementation(APawn* InstigatorPawn)
{
	if (InstigatorPawn)
	{
		if (ADaPlayerState* PlayerState = InstigatorPawn->GetInstigatorController()->GetPlayerState<ADaPlayerState>())
		{
			PlayerState->AdjustCredits(CreditsAmount);
					
			HideAndCooldownItem(InstigatorPawn);
		}
	}
}

FText ADaCreditsPickupItem::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	return FText::Format(LOCTEXT("CreditsPickup_InteractMessage", "Gain {0} Credits."), CreditsAmount);
}

#undef LOCTEXT_NAMESPACE