// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DaActionEffect.h"
#include "DaActionEffect_Thorns.generated.h"

class UDaAttributeComponent;
/**
 * 
 */
UCLASS()
class DRAGONROGUE_API UDaActionEffect_Thorns : public UDaActionEffect
{
	GENERATED_BODY()

public:

	virtual void StartAction_Implementation(AActor* Instigator) override;

	virtual void StopAction_Implementation(AActor* Instigator) override;

	UDaActionEffect_Thorns();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	float ReflectedDamagePct;
	
	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, UDaAttributeComponent* OwningComp, float NewHealth, float Delta);
};
