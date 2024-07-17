// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DaAICharacter.generated.h"

class UDaAttributeComponent;
class UPawnSensingComponent;
// AIPerception could also work... it was supposed to replace PawnSensing

UCLASS()
class DRAGONROGUE_API ADaAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ADaAICharacter();

protected:

	virtual void PostInitializeComponents() override;
	
	UPROPERTY(VisibleAnywhere, Category="Components")
	UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UDaAttributeComponent* AttributeComp;
	
	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, UDaAttributeComponent* OwningComp, float NewHealth, float Delta);
};
