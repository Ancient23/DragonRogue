// Fill out your copyright notice in the Description page of Project Settings.


#include "DaActionPickupItem.h"

#include "DaActionComponent.h"

void ADaActionPickupItem::ActOnInteraction(AActor* InstigatorActor)
{
	if (InstigatorActor)
	{
		UDaActionComponent* ActionComp = UDaActionComponent::GetActions(InstigatorActor);
		if (ActionComp && !ActionComp->ContainsActionWithName(ActionName))
		{
			ActionComp->AddAction(InstigatorActor, ActionClass);

			Super::ActOnInteraction(InstigatorActor);
		}
	}
}
