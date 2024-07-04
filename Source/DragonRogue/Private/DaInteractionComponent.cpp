// Fill out your copyright notice in the Description page of Project Settings.


#include "DaInteractionComponent.h"

#include "DaGameplayInterface.h"

// Sets default values for this component's properties
UDaInteractionComponent::UDaInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDaInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDaInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDaInteractionComponent::PrimaryInteract()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	AActor* MyOwner = GetOwner();

	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	
	FVector End = EyeLocation + (EyeRotation.Vector()*1000);

	FHitResult hit;
	GetWorld()->LineTraceSingleByObjectType(hit, EyeLocation, End, ObjectQueryParams);

	if (AActor* HitActor = hit.GetActor())
	{
		if (HitActor->Implements<UDaGameplayInterface>())
		{
			if (APawn* MyPawn = Cast<APawn>(MyOwner))
			{
				IDaGameplayInterface::Execute_Interact(HitActor, MyPawn);
			}
		}
	}

	DrawDebugLine(GetWorld(), EyeLocation, End, FColor::Red, false, 2.0f, 0, 2.0f);
}
