// Fill out your copyright notice in the Description page of Project Settings.


#include "DaProjectile.h"
#include "DaAttributeComponent.h"
#include "DaGameplayFunctionLibrary.h"

#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "DragonRogue/DragonRogue.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ADaProjectile::ADaProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionProfileName(TEXT("Projectile"));
	SphereComp->OnComponentHit.AddDynamic(this, &ADaProjectile::OnActorHit);
	RootComponent = SphereComp;

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EffectComp"));
	EffectComp->SetupAttachment(RootComponent);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;
	MovementComp->ProjectileGravityScale = 0.0f;
	MovementComp->InitialSpeed = 8000;

	FlightSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("FlightSoundComp"));
	FlightSoundComp->SetupAttachment(RootComponent);
	
	ImpactShakeInnerRadius = 0.0f;
	ImpactShakeOuterRadius = 1500.0f;

	SetReplicates(true);
}

void ADaProjectile::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Explode();
}

void ADaProjectile::Explode_Implementation()
{
	if (ensure(IsValid(this)))
	{
		FlightSoundComp->Stop();
		
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), GetActorRotation());
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), ImpactShake, GetActorLocation(), ImpactShakeInnerRadius, ImpactShakeOuterRadius);

		Destroy();
	}
}

void ADaProjectile::ProjectileWillLaunch()
{
}

// Called when the game starts or when spawned
void ADaProjectile::BeginPlay()
{
	Super::BeginPlay();

	ProjectileWillLaunch();
	
	FlightSoundComp->Play();
	
	if (APawn *InstigatorActor = GetInstigator())
		SphereComp->IgnoreActorWhenMoving(InstigatorActor, true);
	
}
