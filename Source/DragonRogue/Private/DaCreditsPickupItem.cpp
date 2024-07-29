// Fill out your copyright notice in the Description page of Project Settings.


#include "DaCreditsPickupItem.h"

#include "DaPlayerState.h"


ADaCreditsPickupItem::ADaCreditsPickupItem()
{
	CreditsAmount = 10.0f;
}

void ADaCreditsPickupItem::ActOnInteraction(AActor* InstigatorActor)
{
	if (InstigatorActor)
	{
		if (ADaPlayerState* PlayerState = InstigatorActor->GetInstigatorController()->GetPlayerState<ADaPlayerState>())
		{
			PlayerState->AdjustCredits(CreditsAmount);
					
			// super class handles effects if this was used so if no health change was made dont call super
			Super::ActOnInteraction(InstigatorActor);
		}
	}
}
