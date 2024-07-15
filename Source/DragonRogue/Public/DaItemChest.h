// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DaGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "DaItemChest.generated.h"

UCLASS()
class DRAGONROGUE_API ADaItemChest : public AActor, public IDaGameplayInterface
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere)
	float TargetPitch;
	
	void Interact_Implementation(APawn* InstigatorPawn) override;
	
protected:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UStaticMeshComponent* LidMesh;

public:	
	
	// Sets default values for this actor's properties
	ADaItemChest();
};
