// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DaProjectile.h"
#include "GameFramework/Actor.h"
#include "DaMagicProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;

UCLASS()
class DRAGONROGUE_API ADaMagicProjectile : public ADaProjectile
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	USphereComponent* SphereComp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UProjectileMovementComponent* MovementComp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UParticleSystemComponent* EffectComp;
	
	// Sets default values for this actor's properties
	ADaMagicProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
