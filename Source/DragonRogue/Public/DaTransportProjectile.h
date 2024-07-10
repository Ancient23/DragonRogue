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

public:
	ADaTransportProjectile();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UParticleSystem* TransportEffect;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	FTimerHandle TimerHandle_Transport;
	float ElapsedTime;
	bool bExploded;
	
	void DoTeleport();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
