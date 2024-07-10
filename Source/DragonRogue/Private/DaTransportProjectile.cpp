// Fill out your copyright notice in the Description page of Project Settings.


#include "DaTransportProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

ADaTransportProjectile::ADaTransportProjectile()
{
	MovementComp->InitialSpeed = 1000.0f;
	ElapsedTime = 0.0f;
	bExploded = false;
	
	TransportEffect = CreateDefaultSubobject<UParticleSystem>(TEXT("TransportEffect"));

}

void ADaTransportProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ElapsedTime += DeltaTime;
	
	if (ElapsedTime >= 0.2f && !bExploded)
	{
		bExploded = true;
		MovementComp->StopMovementImmediately();
		FVector ProjectileLocation = GetActorLocation();
		FRotator ProjectileRotation = GetActorRotation();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TransportEffect, ProjectileLocation, ProjectileRotation);
		
		GetWorldTimerManager().SetTimer(TimerHandle_Transport, this, &ADaTransportProjectile::DoTeleport, 0.2f, false);
	}
}

void ADaTransportProjectile::DoTeleport()
{
	FVector ProjectileLocation = GetActorLocation();
	if (APawn *InstigatorActor = GetInstigator())
	{
		InstigatorActor->SetActorLocation(ProjectileLocation);
	}
	Destroy();
}

void ADaTransportProjectile::BeginPlay()
{
	Super::BeginPlay();
	ElapsedTime = 0.0f;
	
}