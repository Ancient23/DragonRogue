// Fill out your copyright notice in the Description page of Project Settings.


#include "DaProjectile.h"
#include "DaAttributeComponent.h"

#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
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

	DamageAmount = 0.f;
}

void ADaProjectile::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ApplyDamage(OtherActor, true);
}

void ADaProjectile::ApplyDamage(AActor* OtherActor, bool ShouldExplode)
{
	if (OtherActor && OtherActor != GetInstigator() && DamageAmount>0.f)
	{
		if (UDaAttributeComponent* AttributeComp = Cast<UDaAttributeComponent>(OtherActor->GetComponentByClass(UDaAttributeComponent::StaticClass())))
		{
			AttributeComp->ApplyHealthChange(-DamageAmount);
		}
	}

	if(ShouldExplode) Explode();
}

void ADaProjectile::Explode_Implementation()
{
	if (IsValid(this))
	{
		FlightSoundComp->Stop();
		
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), GetActorRotation());
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());

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
