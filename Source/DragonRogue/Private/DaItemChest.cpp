// Fill out your copyright notice in the Description page of Project Settings.


#include "DaItemChest.h"

#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "InteractableActors"

// Sets default values
ADaItemChest::ADaItemChest()
{
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidMesh"));
	LidMesh->SetupAttachment(BaseMesh);

	TargetPitch = 110;

	bReplicates = true;
}

void ADaItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	bLidOpened = !bLidOpened;
	OnRep_LidOpened();
}

FText ADaItemChest::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	if (bLidOpened)
	{
		return LOCTEXT("Chest_InteractMessage", "Close Chest");
	}
	else
	{
		return LOCTEXT("Chest_InteractMessage", "Open Chest");
	}
}

void ADaItemChest::OnActorLoaded_Implementation()
{
	OnRep_LidOpened();
}

void ADaItemChest::OnRep_LidOpened()
{
	float CurrentPitch = bLidOpened ? TargetPitch : 0.0f;
	LidMesh->SetRelativeRotation(FRotator(CurrentPitch, 0, 0));
}

void ADaItemChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADaItemChest, bLidOpened);
}

#undef LOCTEXT_NAMESPACE