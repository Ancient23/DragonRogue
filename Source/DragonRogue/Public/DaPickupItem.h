// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DaGameplayInterface.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DaPickupItem.generated.h"

class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActiveStateChanged, AActor*, InstigatorActor, bool, IsActive);


UCLASS()
class DRAGONROGUE_API ADaPickupItem : public AActor, public IDaGameplayInterface
{
	GENERATED_BODY()
	
public:
	
	void Interact_Implementation(APawn* InstigatorPawn) override;

	virtual FText GetInteractText_Implementation(APawn* InstigatorPawn) override;
	
	ADaPickupItem();
	
	UPROPERTY(BlueprintAssignable, Category="Pickups")
	FOnActiveStateChanged OnActiveStateChanged;

protected:

	UPROPERTY(VisibleAnywhere, Category="Effects")
	FName TimeToHitParamName;

	UPROPERTY(VisibleAnywhere, Category="Effects")
	FName HitFlashColorParamName;

	UPROPERTY(VisibleAnywhere, Category="Effects")
	FName AlphaVisibilityParamName;

	UPROPERTY(EditAnywhere, Category="Effects")
	FVector FlashColor;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Components")
	USphereComponent* SphereComp;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Components")
	UStaticMeshComponent* BaseMeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Respawn Settings")
	bool bShouldRespawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Respawn Settings")
	bool bShouldRespawnOnDeath;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Respawn Settings")
	float RespawnDelay;

	UFUNCTION()
	void OnPlayerRespawn(APawn* OldPawn, APawn* NewPawn);

	UPROPERTY(Replicated)
	bool bIsActive;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastActiveStateChanged(AActor* InstigatorActor, bool NewState);
	
	UFUNCTION()
	void FadeMesh(AActor* InstigatorActor);

	UFUNCTION()
	void ShowItem(APawn* InstigatorPawn);

	void HideAndCooldownItem(APawn* InstigatorPawn);

	void SetItemState(APawn* InstigatorPawn, bool bNewIsActive);
	
};


