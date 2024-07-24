// Fill out your copyright notice in the Description page of Project Settings.


#include "DaHealthPickupItem.h"

#include "DaAttributeComponent.h"
#include "DaGameplayFunctionLibrary.h"

ADaHealthPickupItem::ADaHealthPickupItem()
{
	HealthAmount = 10.f;
}

void ADaHealthPickupItem::ActOnInteraction(AActor* InstigatorActor)
{
	if (InstigatorActor)
	{
		if (UDaGameplayFunctionLibrary::ApplyHealing(this, InstigatorActor, HealthAmount))
		{
			// super class handles effects if this was used so if no health change was made dont call super
			Super::ActOnInteraction(InstigatorActor);
		}
	}
}