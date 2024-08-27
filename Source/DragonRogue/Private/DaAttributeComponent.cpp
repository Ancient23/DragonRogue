// Fill out your copyright notice in the Description page of Project Settings.


#include "DaAttributeComponent.h"

#include "DaGameModeBase.h"
#include "DragonRogue/DragonRogue.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("da.DamageMultiplier"), 1.0f, TEXT("Global Damage Modifier for Attribute Component"), ECVF_Cheat);

// Sets default values
UDaAttributeComponent::UDaAttributeComponent()
{
	Health = 100.0f;
	HealthMax = 200.0f;
	LowHealthThreshold = 20.0f;

	Rage = 0.0f;
	RageMax = 100.0f;
	RageMultiplier = 0.5f; // multiplied by amount in AddRage
	
	SetIsReplicatedByDefault(true);
}

void UDaAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDaAttributeComponent, Health);
	DOREPLIFETIME(UDaAttributeComponent, HealthMax);
	DOREPLIFETIME(UDaAttributeComponent, LowHealthThreshold);
	
	DOREPLIFETIME(UDaAttributeComponent, Rage);
	DOREPLIFETIME(UDaAttributeComponent, RageMax);
	DOREPLIFETIME(UDaAttributeComponent, RageMultiplier);
	
	//@TODO: Network Optimization using COND... 
	//DOREPLIFETIME_CONDITION(UDaAttributeComponent, HealthMax, COND_InitialOnly);
}


UDaAttributeComponent* UDaAttributeComponent::GetAttributes(AActor* FromActor)
{
	if (FromActor)
	{
		return Cast<UDaAttributeComponent>(FromActor->FindComponentByClass<UDaAttributeComponent>());
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
	if (!GetOwner()->CanBeDamaged() && Delta < 0.0f)
	{
		// God Mode, allow healing
		return false;
	}
	
	if (Delta < 0.0f)
	{
		// Globally increase all damage using CVar multiplier
		float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();
		if (DamageMultiplier != 1.0f)
		{
			LOG_WARNING("Global Damage Multiplier set to %f", DamageMultiplier);
			Delta *= DamageMultiplier;
		}
	}

	if (!IsAlive())
	{
		return 0.0f;
	}
	
	float OldHealth = Health;
	float NewHealth = FMath::Clamp(Health+Delta, 0, HealthMax);
	float ActualDelta = NewHealth - OldHealth;

	if (GetOwner()->HasAuthority())
	{
		Health = NewHealth;

		if (!FMath::IsNearlyZero(ActualDelta))
		{
			MulticastHealthChanged(InstigatorActor, Health, ActualDelta);
		}

		if (ActualDelta < 0.0f && FMath::IsNearlyZero(Health))
		{
			ADaGameModeBase* Gamemode = GetWorld()->GetAuthGameMode<ADaGameModeBase>();
			if (Gamemode)
			{
				// catch any issues where actions or effects (like thorns on both killer and victim) may be causing infinite damage or damage reflections
				ensureAlways(Health==NewHealth);
				Gamemode->OnActorKilled(GetOwner(), InstigatorActor);
			}
		}
	}
	
	return !FMath::IsNearlyZero(ActualDelta);
}

void UDaAttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, float NewHealth, float Delta)
{
	OnHealthChanged.Broadcast(InstigatorActor, this, NewHealth, Delta);
}

void UDaAttributeComponent::MulticastRageChanged_Implementation(AActor* InstigatorActor, float NewRage, float Delta)
{
	OnRageChanged.Broadcast(InstigatorActor, this, NewRage, Delta);
}

float UDaAttributeComponent::CalculateRage(float CurrentRage, float Delta) const
{
	return FMath::Clamp(CurrentRage + Delta*RageMultiplier, 0, RageMax);
}

void UDaAttributeComponent::AddRage(float Amount)
{
	if (ensure(Amount > 0.0f))
	{
		float NewRage = CalculateRage(Rage, Amount);
		float ActualDelta = NewRage - Rage;
		if (ActualDelta > 0.0f)
		{
			Rage = NewRage;
			OnRageChanged.Broadcast(GetOwner(), this, NewRage, ActualDelta);
		}
	}
}

void UDaAttributeComponent::UseRage(float Amount)
{
	if (ensure(Amount > 0.0f))
	{
		float NewRage = CalculateRage(Rage, -1*Amount);
		float ActualDelta = Rage - NewRage;
		if (ActualDelta > 0.0f)
		{
			Rage = NewRage;
			OnRageChanged.Broadcast(GetOwner(), this, NewRage, -ActualDelta);
		}
	}
}

void UDaAttributeComponent::SetRageToMax()
{
	if (Rage < RageMax)
	{
		AddRage(RageMax - Rage);
	}
}

float UDaAttributeComponent::GetRageMax() const
{
	return RageMax;
}

float UDaAttributeComponent::GetRage() const
{
	return Rage;
}

bool UDaAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -HealthMax);
}

bool UDaAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

bool UDaAttributeComponent::LowHealth() const
{
	return Health <= LowHealthThreshold;
}

bool UDaAttributeComponent::IsFullHealth() const
{
	return FMath::IsNearlyEqual(Health, HealthMax);
}

float UDaAttributeComponent::GetHealth() const
{
	return Health;
}

float UDaAttributeComponent::GetHealthMax() const
{
	return HealthMax;
}

