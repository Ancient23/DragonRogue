// Fill out your copyright notice in the Description page of Project Settings.


#include "DaGameplayFunctionLibrary.h"
#include "DaAttributeComponent.h"

static TAutoConsoleVariable<float> CVarDamageImpulseMultiplier(TEXT("da.DamageImpulseMultiplier"), 1.0f, TEXT("Global Damage Impulse Modifier"), ECVF_Cheat);

bool UDaGameplayFunctionLibrary::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount)
{
	UDaAttributeComponent* AttributeComponent = UDaAttributeComponent::GetAttributes(TargetActor);
	if (AttributeComponent)
	{
		return AttributeComponent->ApplyHealthChange(DamageCauser, -DamageAmount);
	}
	return false;
}

bool UDaGameplayFunctionLibrary::ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount,
	const FHitResult& HitResult)
{
	if (ApplyDamage(DamageCauser, TargetActor, DamageAmount))
	{
		float ImpulseMultiplier = CVarDamageImpulseMultiplier.GetValueOnGameThread();
		UPrimitiveComponent* HitComp = HitResult.GetComponent();
		if (HitComp && HitComp->IsSimulatingPhysics(HitResult.BoneName))
		{
			// Was using -HitResult.ImpactNormal, but calculating direction of the hit it more accurate
			// Direction = Target - Origin 
			FVector Direction = HitResult.TraceEnd - HitResult.TraceStart;
			Direction.Normalize();
			
			HitComp->AddImpulseAtLocation(Direction * 100000.0f*ImpulseMultiplier, HitResult.ImpactPoint, HitResult.BoneName);
		}
		return true;
	}

	return false;
}

bool UDaGameplayFunctionLibrary::ApplyHealing(AActor* Healer, AActor* TargetActor, float HealingAmount)
{
	UDaAttributeComponent* AttributeComponent = UDaAttributeComponent::GetAttributes(TargetActor);
	if (AttributeComponent)
	{
		return AttributeComponent->ApplyHealthChange(Healer, HealingAmount);
	}
	return false;
}

bool UDaGameplayFunctionLibrary::ApplyHealingMax(AActor* Healer, AActor* TargetActor)
{
	UDaAttributeComponent* AttributeComponent = UDaAttributeComponent::GetAttributes(TargetActor);
	if (AttributeComponent)
	{
		return ApplyHealing(Healer, TargetActor, AttributeComponent->GetHealthMax());
	}
	return false;
}
