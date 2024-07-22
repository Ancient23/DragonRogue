// Fill out your copyright notice in the Description page of Project Settings.


#include "DaAttributeComponent.h"

#include "DaGameModeBase.h"


// Sets default values
UDaAttributeComponent::UDaAttributeComponent()
{
	Health = 100.0f;
	HealthMax = 200.0f;
	LowHealthThreshold = 20.0f;
	
	bIsAlive = true;
}

UDaAttributeComponent* UDaAttributeComponent::GetAttributes(AActor* FromActor)
{
	if (FromActor)
	{
		return Cast<UDaAttributeComponent>(FromActor->GetComponentByClass(UDaAttributeComponent::StaticClass()));
	}

	return nullptr;
}

bool UDaAttributeComponent::IsActorAlive(AActor* Actor)
{
	UDaAttributeComponent* AttributeComp = GetAttributes(Actor);
	if (AttributeComp)
	{
		return AttributeComp->IsAlive();
	}

	return false;
}

bool UDaAttributeComponent::HasLowHealth(AActor* Actor)
{
	UDaAttributeComponent* AttributeComp = GetAttributes(Actor);
	if (AttributeComp)
	{
		return AttributeComp->LowHealth();
	}

	return false;
}

bool UDaAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	if (!GetOwner()->CanBeDamaged())
	{
		// God Mode
		return false;
	}
	
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
	OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta);

	if (ActualDelta < 0.0f && Health <= 0.f)
	{
		// dead
		bIsAlive = false;

		ADaGameModeBase* Gamemode = GetWorld()->GetAuthGameMode<ADaGameModeBase>();
		if (Gamemode)
		{
			Gamemode->OnActorKilled(GetOwner(), InstigatorActor);
		}
	}
	else if (OldHealth <= 0.f && Health > 0)
	{
		// revived
		bIsAlive = true;
	}
	
	return ActualDelta != 0;
}

bool UDaAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -HealthMax);
}

bool UDaAttributeComponent::IsAlive() const
{
	return bIsAlive;
}

bool UDaAttributeComponent::LowHealth() const
{
	return Health <= LowHealthThreshold;
}

float UDaAttributeComponent::GetHealth() const
{
	return Health;
}

float UDaAttributeComponent::GetHealthMax() const
{
	return HealthMax;
}

