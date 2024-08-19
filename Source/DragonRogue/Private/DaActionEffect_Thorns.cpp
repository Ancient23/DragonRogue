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
	AActor* OwningActor = GetOwningComponent()->GetOwner();
	
	if (Delta >= 0 || InstigatorActor == OwningActor)
	{
		// Dont reflect damage if owner caused itself damage
		return;
	}
	
	int32 DamageToReflect = FMath::RoundToInt32(Delta * ReflectedDamagePct/100.0f);
	if (DamageToReflect == 0)
	{
		// Reducing the value will allow thorns effect on both instigator and owner to not go into an infinite damage loop
		return;
	}	
	DamageToReflect = FMath::Abs(DamageToReflect);

	//FString DebugMsg = FString::Printf(TEXT("Applying Thorns damage %i to"), DamageToReflect) + " " + GetNameSafe(InstigatorActor);
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, DebugMsg);
	
	UDaGameplayFunctionLibrary::ApplyDamage(OwningActor, InstigatorActor, DamageToReflect);
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



