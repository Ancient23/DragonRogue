// Fill out your copyright notice in the Description page of Project Settings.


#include "DaHealthPickupItem.h"

#include "DaAttributeComponent.h"

ADaHealthPickupItem::ADaHealthPickupItem()
{
	HealthAmount = 10.f;
}

void ADaHealthPickupItem::ActOnInteraction(AActor* InstigatorActor)
{
	Super::ActOnInteraction(InstigatorActor);
	if (InstigatorActor)
	{
		if (UDaAttributeComponent* AttributeComp = Cast<UDaAttributeComponent>(InstigatorActor->GetComponentByClass(UDaAttributeComponent::StaticClass())))
		{
			AttributeComp->ApplyHealthChange(HealthAmount);
		}
	}
}