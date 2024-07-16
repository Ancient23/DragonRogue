// Fill out your copyright notice in the Description page of Project Settings.


#include "DaPickupItem.h"

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

	RespawnDelay = 10.0f;
	bIsActive = true;
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
		
		BaseMeshComp->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
		BaseMeshComp->SetVectorParameterValueOnMaterials("FlashColor", FVector(UE::Geometry::LinearColors::Green3f()));

		// disable for a time, then re-enable after a time
		FTimerHandle TimerHandle_DelayedHide;
		GetWorldTimerManager().SetTimer(TimerHandle_DelayedHide, this, &ADaPickupItem::FadeMesh, 0.25f );
		
		// start a 10 second timer to re-enable item if its still available to be picked up
		FTimerHandle TimerHandle_DelayedActivate;
		GetWorldTimerManager().SetTimer(TimerHandle_DelayedActivate, this, &ADaPickupItem::RespawnItem, RespawnDelay );

	}
}

void ADaPickupItem::FadeMesh()
{
	BaseMeshComp->SetScalarParameterValueOnMaterials("AlphaVisible", 0.1f);
	EffectComp->Deactivate();
	IdleSoundComp->Stop();
}

void ADaPickupItem::RespawnItem()
{
	BaseMeshComp->SetScalarParameterValueOnMaterials("AlphaVisible", 1.f);
	bIsActive = true;
	EffectComp->Activate();
	IdleSoundComp->Play();

}
