// Fill out your copyright notice in the Description page of Project Settings.


#include "DaHealthPickupItem.h"

#include "DaGameplayFunctionLibrary.h"
#include "DaPlayerState.h"

ADaHealthPickupItem::ADaHealthPickupItem()
{
	HealthAmount = 10.f;
	Cost = 5.0f;
}

void ADaHealthPickupItem::ActOnInteraction(AActor* InstigatorActor)
{
	if (InstigatorActor)
	{
		if (ADaPlayerState* PlayerState = InstigatorActor->GetInstigatorController()->GetPlayerState<ADaPlayerState>())
		{
			if (PlayerState->GetCredits() >= Cost)
			{
				if (UDaGameplayFunctionLibrary::ApplyHealing(this, InstigatorActor, HealthAmount))
				{
					PlayerState->AdjustCredits(-Cost);
					
					// super class handles effects if this was used so if no health change was made dont call super
					Super::ActOnInteraction(InstigatorActor);
				}
			}
		}
	}
}