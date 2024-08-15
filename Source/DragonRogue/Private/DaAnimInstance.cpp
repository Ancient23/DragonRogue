// Fill out your copyright notice in the Description page of Project Settings.


#include "DaAnimInstance.h"

#include "DaActionComponent.h"
#include "GameplayTagContainer.h"

void UDaAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	AActor* OwningActor = GetOwningActor();
	if (OwningActor)
	{
		ActionComp = Cast<UDaActionComponent>(OwningActor->GetComponentByClass(UDaActionComponent::StaticClass()));
	}
}

void UDaAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	static FGameplayTag StunnedTag = FGameplayTag::RequestGameplayTag("Status.Stunned");
	if (ActionComp)
	{
		bIsStunned = ActionComp->ActiveGameplayTags.HasTag(StunnedTag);
	}
}
