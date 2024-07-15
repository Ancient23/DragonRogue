// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DaGameplayInterface.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DaPickupItem.generated.h"

class USphereComponent;

UCLASS()
class DRAGONROGUE_API ADaPickupItem : public AActor, public IDaGameplayInterface
{
	GENERATED_BODY()
	
public:
	
	void Interact_Implementation(APawn* InstigatorPawn) override;
	
	// Sets default values for this actor's properties
	ADaPickupItem();

protected:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Components")
	USphereComponent* SphereComp;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Components")
	UStaticMeshComponent* BaseMeshComp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Components")
	UParticleSystemComponent* EffectComp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Components")
	UAudioComponent* IdleSoundComp;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* PickupVFX;

	UPROPERTY(EditDefaultsOnly, Category= "Sound FX")
	USoundBase* PickupSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Respawn Settings")
	float RespawnDelay;

	bool bIsActive;
	
	virtual void ActOnInteraction(AActor* InstigatorActor);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	void FadeMesh();
	void RespawnItem();
};


