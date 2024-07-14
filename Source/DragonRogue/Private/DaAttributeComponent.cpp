// Fill out your copyright notice in the Description page of Project Settings.


#include "DaAttributeComponent.h"


// Sets default values
UDaAttributeComponent::UDaAttributeComponent()
{
	Health = 100.0f;
	HealthMax = 200.0f;
}



bool UDaAttributeComponent::ApplyHealthChange(float Delta)
{
	Health += Delta;
	Health = FMath::Clamp(Health, 0, HealthMax);
	
	OnHealthChanged.Broadcast(nullptr, this, Health, Delta);
	
	return true;
}

bool UDaAttributeComponent::IsAlive() const
{
	return Health > 0.f;
}

