// Fill out your copyright notice in the Description page of Project Settings.


#include "DaPickupItem.h"

#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Util/ColorConstants.h"

// Sets default values
ADaPickupItem::ADaPickupItem()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;
	
	BaseMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMeshComp"));
	// disable Collision for this since we just use Interact interface
	BaseMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BaseMeshComp->SetupAttachment(RootComponent);

	bShouldRespawn = true;
	bShouldRespawnOnDeath = false;
	RespawnDelay = 10.0f;
	bIsActive = true;

	TimeToHitParamName = "TimeToHit";
	HitFlashColorParamName = "FlashColor";
	AlphaVisibilityParamName = "AlphaVisible";

	FlashColor = FVector(UE::Geometry::LinearColors::Green3f());

	bReplicates = true;
}

void ADaPickupItem::Interact_Implementation(APawn* InstigatorPawn)
{
	// Derived Classes to override
}

void ADaPickupItem::MulticastActiveStateChanged_Implementation(AActor* InstigatorActor, bool NewState)
{
	if (!NewState)
	{
		BaseMeshComp->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
		BaseMeshComp->SetVectorParameterValueOnMaterials(HitFlashColorParamName, FlashColor);
	
		// disable for a time, then re-enable if requested
		FTimerHandle TimerHandle_DelayedHide;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "FadeMesh", InstigatorActor);

		GetWorldTimerManager().SetTimer(TimerHandle_DelayedHide, Delegate, 0.25f, false );
	}
	else
	{
		SetActorEnableCollision(true);
		// Set visibility on root and all children
		RootComponent->SetVisibility(true, true);
		
		// broadcast to listeners
		OnActiveStateChanged.Broadcast(InstigatorActor, true);
	}
}

void ADaPickupItem::FadeMesh(AActor* InstigatorActor)
{
	SetActorEnableCollision(false);
	// Set visibility on root and all children
	RootComponent->SetVisibility(false, true);

	// broadcast to listeners
	OnActiveStateChanged.Broadcast(InstigatorActor, false);
}

void ADaPickupItem::ShowItem(APawn* InstigatorPawn)
{
	SetItemState(InstigatorPawn, true);
}

void ADaPickupItem::OnPlayerRespawn(APawn* OldPawn, APawn* NewPawn)
{
	// start a 10 second timer to re-enable item if its still available to be picked up
	FTimerHandle TimerHandle_DelayedActivate;
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "ShowItem", NewPawn);
	GetWorldTimerManager().SetTimer(TimerHandle_DelayedActivate, Delegate, RespawnDelay, false );
}

void ADaPickupItem::HideAndCooldownItem(APawn* InstigatorPawn)
{
	SetItemState(InstigatorPawn, false);

	if (bShouldRespawnOnDeath)
	{
		Cast<ACharacter>(InstigatorPawn)->GetController()->OnPossessedPawnChanged.AddDynamic(this, &ADaPickupItem::OnPlayerRespawn);
	}
	// destroy if this wont respawn
	else if (bShouldRespawn)
	{
		// start a 10 second timer to re-enable item if its still available to be picked up
		FTimerHandle TimerHandle_DelayedActivate;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "ShowItem", InstigatorPawn);
		GetWorldTimerManager().SetTimer(TimerHandle_DelayedActivate, Delegate, RespawnDelay, false );
	}
	else
	{
		Destroy();
	}
}

void ADaPickupItem::SetItemState(APawn* InstigatorPawn, bool bNewIsActive)
{
	bIsActive = bNewIsActive;
	MulticastActiveStateChanged(InstigatorPawn, bNewIsActive);
}

void ADaPickupItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADaPickupItem, bIsActive);
}
