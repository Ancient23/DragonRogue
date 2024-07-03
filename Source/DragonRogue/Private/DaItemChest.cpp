// Fill out your copyright notice in the Description page of Project Settings.


#include "DaItemChest.h"

// Sets default values
ADaItemChest::ADaItemChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidMesh"));
	LidMesh->SetupAttachment(BaseMesh);

	TargetPitch = 110;
}

void ADaItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	IDaGameplayInterface::Interact_Implementation(InstigatorPawn);

	LidMesh->SetRelativeRotation(FRotator(TargetPitch, 0, 0));
}

// Called when the game starts or when spawned
void ADaItemChest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADaItemChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

