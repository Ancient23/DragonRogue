// Fill out your copyright notice in the Description page of Project Settings.


#include "DaMagicProjectile.h"

#include "DaAttributeComponent.h"
#include "DaCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADaMagicProjectile::ADaMagicProjectile()
{
	MovementComp->InitialSpeed = 1000.0f;
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ADaMagicProjectile::OnActorOverlap);

	DamageAmount = 20.f;
}

void ADaMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ApplyDamage(OtherActor, true);
}


void ADaMagicProjectile::ProjectileWillLaunch()
{
	Super::ProjectileWillLaunch();

	ADaCharacter* Character = Cast<ADaCharacter>(GetInstigator());
	if (ensure(Character))
	{
		UGameplayStatics::SpawnEmitterAttached(LaunchVFX, Character->GetMesh(), TEXT("Muzzle_01"), Character->GetActorLocation(), Character->GetActorRotation());
	}
}
