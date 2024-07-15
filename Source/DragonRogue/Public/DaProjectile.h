// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DaProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;
class UAudioComponent;

UCLASS(Abstract)
class DRAGONROGUE_API ADaProjectile : public AActor
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Components")
	USphereComponent* SphereComp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Components")
	UProjectileMovementComponent* MovementComp;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Components")
	UParticleSystemComponent* EffectComp;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UAudioComponent* FlightSoundComp;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* ImpactVFX;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* LaunchVFX;
	
	UPROPERTY(EditDefaultsOnly, Category= "Sound FX")
	USoundBase* ImpactSound;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UCameraShakeBase> CameraHitVFX;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Damage")
	float DamageAmount;
	
	UFUNCTION()
	virtual void OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Helper to apply damage to anything the projectile hits, the other actor
	void ApplyDamage(AActor* OtherActor, bool ShouldExplode);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Explode();

	virtual void ProjectileWillLaunch();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Sets default values for this actor's properties
	ADaProjectile();

};
