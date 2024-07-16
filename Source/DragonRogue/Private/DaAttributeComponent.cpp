// Fill out your copyright notice in the Description page of Project Settings.


#include "DaAttributeComponent.h"


// Sets default values
UDaAttributeComponent::UDaAttributeComponent()
{
	Health = 100.0f;
	HealthMax = 200.0f;
	bIsAlive = true;
}

bool UDaAttributeComponent::ApplyHealthChange(float Delta)
{
	if (Delta < 0)
	{
		// damage
		if (Health <= 0.f)
		{
			// Do nothing
			return false;
		}
	}
	else if (Delta > 0)
	{
		// Healing
		if (Health >= HealthMax)
		{
			return false;
		}
	}
	else
	{
		// 0 do nothing
		return false;
	}
	float OldHealth = Health;
	Health += Delta;
	Health = FMath::Clamp(Health, 0, HealthMax);

	float ActualDelta = Health - OldHealth;
	OnHealthChanged.Broadcast(nullptr, this, Health, ActualDelta);

	if (Health <= 0.f)
	{
		// dead
		bIsAlive = false;
	}
	else if (OldHealth <= 0.f && Health > 0)
	{
		// revived
		bIsAlive = true;
	}
	
	return ActualDelta != 0;
}

bool UDaAttributeComponent::IsAlive() const
{
	return bIsAlive;
}

