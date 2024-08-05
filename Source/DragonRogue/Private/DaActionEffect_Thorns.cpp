// Fill out your copyright notice in the Description page of Project Settings.


#include "DaActionEffect_Thorns.h"

#include "DaActionComponent.h"
#include "DaAttributeComponent.h"
#include "DaGameplayFunctionLibrary.h"

UDaActionEffect_Thorns::UDaActionEffect_Thorns()
{
	ReflectedDamagePct = 25.0f;
	
	Duration = 0.f;
	Period = 0.f;
}

void UDaActionEffect_Thorns::OnHealthChanged(AActor* InstigatorActor, UDaAttributeComponent* OwningComp,
	float NewHealth, float Delta)
{
	if (InstigatorActor == OwningComp->GetOwner())
	{
		// Dont reflect damage if owner caused itself damage
		return;
	}
	
	float Damage = FMath::Abs(Delta);
	float DamageToReflect = FMath::RoundToInt32(Damage * ReflectedDamagePct/100.0f);
	if (Damage > 0.f && DamageToReflect == 0.f)
	{
		// If there was some Damage from the Instigator, but it was a low amount (<4), Its possible it got rounded to 0, Lets do 1 point of Damage in this case.
		DamageToReflect = 1.0f;
	}	
	
	//FString DebugMsg = FString::Printf(TEXT("Applying Thorns damage %f to"), DamageToReflect) + " " + GetNameSafe(InstigatorActor);
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, DebugMsg);
	
	UDaGameplayFunctionLibrary::ApplyDamage(OwningComp->GetOwner(), InstigatorActor, DamageToReflect);
}

void UDaActionEffect_Thorns::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	AActor *OwningActor = GetOwningComponent()->GetOwner();
	UDaAttributeComponent* AttributeComp = UDaAttributeComponent::GetAttributes(OwningActor);

	if (AttributeComp)
	{
		AttributeComp->OnHealthChanged.AddDynamic(this, &UDaActionEffect_Thorns::OnHealthChanged);
	}
}

void UDaActionEffect_Thorns::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);

	AActor *OwningActor = GetOwningComponent()->GetOwner();
	UDaAttributeComponent* AttributeComp = UDaAttributeComponent::GetAttributes(OwningActor);

	if (AttributeComp)
	{
		AttributeComp->OnHealthChanged.RemoveDynamic(this, &UDaActionEffect_Thorns::OnHealthChanged);
	}
}



