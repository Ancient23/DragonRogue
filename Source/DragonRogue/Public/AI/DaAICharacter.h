// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DaAICharacter.generated.h"

class UDaActionComponent;
class UDaAttributeComponent;
class UPawnSensingComponent;
class UUserWidget;
class UDaWorldUserWidget;

// AIPerception could also work... it was supposed to replace PawnSensing

UCLASS()
class DRAGONROGUE_API ADaAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ADaAICharacter();

protected:

	UPROPERTY(VisibleDefaultsOnly, Category="UI")
	UDaWorldUserWidget* ActiveHealthBar;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	UPROPERTY(VisibleDefaultsOnly, Category="UI")
	UDaWorldUserWidget* PlayerSeenWidget;
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> PlayerSeenWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	float PlayerSeenEmoteTime;
	
	UPROPERTY(VisibleAnywhere, Category="Effects")
	FName TimeToHitParamName;

	UPROPERTY(VisibleAnywhere, Category="Effects")
	FName HitFlashColorParamName;
	
	UPROPERTY(VisibleAnywhere, Category="Components")
	UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UDaAttributeComponent* AttributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UDaActionComponent* ActionComp;
	
	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, UDaAttributeComponent* OwningComp, float NewHealth, float Delta);

	bool IsKnownTargetActor(AActor* Actor);
	void SetTargetActor(AActor* NewTarget);

	void PlayerSeenWidgetTimeExpired();
	
	virtual void PostInitializeComponents() override;
};
