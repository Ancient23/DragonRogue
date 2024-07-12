// Fill out your copyright notice in the Description page of Project Settings.


#include "DaTransportProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

ADaTransportProjectile::ADaTransportProjectile()
{
	// load particle system
	FString ParticleEffectPath = TEXT("/Game/Content/ExampleContent/Effects/P_Explosion.uasset");
	ImpactVFX = Cast<UParticleSystem>(StaticLoadObject(UParticleSystem::StaticClass(), nullptr, *ParticleEffectPath));

	TeleportDelay = 0.2f;
	DetonateDelay = 0.2f;

	MovementComp->InitialSpeed = 6000.0f;
}

void ADaTransportProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_Detonate, this, &ADaTransportProjectile::Explode, DetonateDelay );
}

void ADaTransportProjectile::Explode_Implementation()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Detonate);

	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());

	EffectComp->DeactivateSystem();
	MovementComp->StopMovementImmediately();
	SetActorEnableCollision(false);

	FTimerHandle TimerHandle_DelayedTeleport;
	GetWorldTimerManager().SetTimer(TimerHandle_DelayedTeleport, this, &ADaTransportProjectile::DoTeleport, DetonateDelay );

	// super destroys so just override so actor can live through timer.
}

void ADaTransportProjectile::DoTeleport()
{
	APawn *InstigatorActor = GetInstigator();
	if (ensure(InstigatorActor))
	{
		// Projectile rotation points actor in new direction but can be jarring
		// FRotator ProjectileRotation = GetActorRotation();
		InstigatorActor->TeleportTo(GetActorLocation(), InstigatorActor->GetActorRotation());
	}
}
