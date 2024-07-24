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
			HitComp->AddImpulseAtLocation(-HitResult.ImpactNormal * 100000.0f*ImpulseMultiplier, HitResult.ImpactPoint, HitResult.BoneName);
		}
		return true;
	}

	return false;
}
