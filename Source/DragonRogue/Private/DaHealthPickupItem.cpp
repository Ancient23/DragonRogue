// Fill out your copyright notice in the Description page of Project Settings.


#include "DaHealthPickupItem.h"

#include "DaAttributeComponent.h"
#include "DaGameplayFunctionLibrary.h"
#include "DaPlayerState.h"

#define LOCTEXT_NAMESPACE "InteractableActors"

ADaHealthPickupItem::ADaHealthPickupItem()
{
	HealthAmount = 10.f;
	Cost = 5.0f;
}

void ADaHealthPickupItem::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn))
	{
		return;
	}

	UDaAttributeComponent* AttributeComp = UDaAttributeComponent::GetAttributes(InstigatorPawn);
	if (ensure(AttributeComp) && !AttributeComp->IsFullHealth())
	{
		ADaPlayerState* PlayerState = InstigatorPawn->GetPlayerState<ADaPlayerState>();
		if (ensure(PlayerState) && PlayerState->GetCredits() >= Cost)
		{
			if (UDaGameplayFunctionLibrary::ApplyHealing(this, InstigatorPawn, HealthAmount))
			{
				PlayerState->AdjustCredits(-Cost);
			
				HideAndCooldownItem(InstigatorPawn);
			}
		}
	}
}

FText ADaHealthPickupItem::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	UDaAttributeComponent* AttributeComp = UDaAttributeComponent::GetAttributes(InstigatorPawn);
	if (AttributeComp && AttributeComp->IsFullHealth())
	{
		return LOCTEXT("HealthPotion_FullHealthWarning", "Health is already full.");
	}

	return FText::Format(LOCTEXT("HealthPotion_InteractMessage", "Cost {0} Credits to restore health to {1}"), Cost, HealthAmount);
}

#undef LOCTEXT_NAMESPACE