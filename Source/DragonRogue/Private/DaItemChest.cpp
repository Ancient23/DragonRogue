// Fill out your copyright notice in the Description page of Project Settings.


#include "DaItemChest.h"

// Sets default values
ADaItemChest::ADaItemChest()
{
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidMesh"));
	LidMesh->SetupAttachment(BaseMesh);

	TargetPitch = 110;
}

void ADaItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	IDaGameplayInterface::Interact_Implementation(InstigatorPawn);

	//LidMesh->SetRelativeRotation(FRotator(TargetPitch, 0, 0));
}

