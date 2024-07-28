// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DaProjectile.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "DaMagicProjectile.generated.h"

UCLASS()
class DRAGONROGUE_API ADaMagicProjectile : public ADaProjectile
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DamageAmount;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FGameplayTag ParryTag;
	
	UPROPERTY(VisibleAnywhere, Category="Effects")
	FName PrimaryHandSocketName;

	UPROPERTY(VisibleAnywhere, Category="Effects")
	FName SecondaryHandSocketName;

	UPROPERTY(VisibleAnywhere, Category="Effects")
	FName EyeSocketName;
	
	UFUNCTION()
	void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	virtual void ProjectileWillLaunch() override;
	
public:
	
	// Sets default values for this actor's properties
	ADaMagicProjectile();
	
};
