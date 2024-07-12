// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DaProjectile.h"
#include "DaTransportProjectile.generated.h"

class UParticleSystem;
/**
 * 
 */
UCLASS()
class DRAGONROGUE_API ADaTransportProjectile : public ADaProjectile
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category="Teleport")
	float TeleportDelay;

	UPROPERTY(EditDefaultsOnly, Category="Teleport")
	float DetonateDelay;
	
	FTimerHandle TimerHandle_Detonate;

	virtual void Explode_Implementation() override;
	void DoTeleport();

	virtual void BeginPlay() override;

public:
	ADaTransportProjectile();

};
