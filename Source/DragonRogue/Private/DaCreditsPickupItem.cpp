// Fill out your copyright notice in the Description page of Project Settings.


#include "DaCreditsPickupItem.h"

#include "DaPlayerState.h"

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
