// Fill out your copyright notice in the Description page of Project Settings.


#include "DaHealthPickupItem.h"

#include "DaGameplayFunctionLibrary.h"
#include "DaPlayerState.h"

ADaHealthPickupItem::ADaHealthPickupItem()
{
	HealthAmount = 10.f;
	Cost = 5.0f;
}

void ADaHealthPickupItem::Interact_Implementation(APawn* InstigatorPawn)
{
	if (InstigatorPawn)
	{
		if (ADaPlayerState* PlayerState = InstigatorPawn->GetInstigatorController()->GetPlayerState<ADaPlayerState>())
		{
			if (PlayerState->GetCredits() >= Cost)
			{
				if (UDaGameplayFunctionLibrary::ApplyHealing(this, InstigatorPawn, HealthAmount))
				{
					PlayerState->AdjustCredits(-Cost);
					
					HideAndCooldownItem(InstigatorPawn);
				}
			}
		}
	}
}
