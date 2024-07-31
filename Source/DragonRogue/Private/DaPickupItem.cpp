// Fill out your copyright notice in the Description page of Project Settings.


#include "DaPickupItem.h"

#include "DaCharacter.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
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

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EffectComp"));
	EffectComp->SetupAttachment(RootComponent);

	IdleSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("IdleSoundComp"));
	IdleSoundComp->SetupAttachment(RootComponent);

	bShouldRespawn = true;
	bShouldRespawnOnDeath = false;
	RespawnDelay = 10.0f;
	bIsActive = true;

	TimeToHitParamName = "TimeToHit";
	HitFlashColorParamName = "FlashColor";
	AlphaVisibilityParamName = "AlphaVisible";

	FlashColor = FVector(UE::Geometry::LinearColors::Green3f());
}

void ADaPickupItem::BeginPlay()
{
	Super::BeginPlay();

	IdleSoundComp->Play();
}

void ADaPickupItem::Interact_Implementation(APawn* InstigatorPawn)
{
	if (bIsActive)
		ActOnInteraction(InstigatorPawn);
}

void ADaPickupItem::ActOnInteraction(AActor* InstigatorActor)
{
	if (IsValid(this))
	{
		bIsActive = false;

		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation(), GetActorRotation());
		UGameplayStatics::SpawnEmitterAtLocation(this, PickupVFX, GetActorLocation(), GetActorRotation());
		
		BaseMeshComp->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
		BaseMeshComp->SetVectorParameterValueOnMaterials(HitFlashColorParamName, FlashColor);

		// disable for a time, then re-enable if requested
		FTimerHandle TimerHandle_DelayedHide;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "FadeMesh", InstigatorActor);

		GetWorldTimerManager().SetTimer(TimerHandle_DelayedHide, Delegate, 0.25f, false );
	}
}

void ADaPickupItem::OnPlayerRespawn(APawn* OldPawn, APawn* NewPawn)
{
	// start a 10 second timer to re-enable item if its still available to be picked up
	FTimerHandle TimerHandle_DelayedActivate;
	GetWorldTimerManager().SetTimer(TimerHandle_DelayedActivate, this, &ADaPickupItem::RespawnItem, RespawnDelay );
}

void ADaPickupItem::FadeMesh(AActor* InstigatorActor)
{
	BaseMeshComp->SetVisibility(false);
	BaseMeshComp->SetScalarParameterValueOnMaterials(AlphaVisibilityParamName, 0.2f);
	EffectComp->Deactivate();
	IdleSoundComp->Stop();
	
	if (bShouldRespawnOnDeath)
	{
		Cast<ADaCharacter>(InstigatorActor)->GetController()->OnPossessedPawnChanged.AddDynamic(this, &ADaPickupItem::OnPlayerRespawn);
	}
	// destroy if this wont respawn
	else if (bShouldRespawn)
	{
		// start a 10 second timer to re-enable item if its still available to be picked up
		FTimerHandle TimerHandle_DelayedActivate;
		GetWorldTimerManager().SetTimer(TimerHandle_DelayedActivate, this, &ADaPickupItem::RespawnItem, RespawnDelay );
	}
	else
	{
		Destroy();
	}
}

void ADaPickupItem::RespawnItem()
{
	BaseMeshComp->SetVisibility(true);
	BaseMeshComp->SetScalarParameterValueOnMaterials(AlphaVisibilityParamName, 1.f);
	bIsActive = true;
	EffectComp->Activate();
	IdleSoundComp->Play();

}
