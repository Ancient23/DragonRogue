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
	if (OtherActor && OtherActor->GetComponentByClass(UDaAttributeComponent::StaticClass()))
	{
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), CameraHitVFX, OtherActor->GetActorLocation(), 100.f, 1000.f);
		ApplyDamage(OtherActor, true);
	}
}


void ADaMagicProjectile::ProjectileWillLaunch()
{
	Super::ProjectileWillLaunch();

	ADaCharacter* Character = Cast<ADaCharacter>(GetInstigator());
	if (IsValid(Character))
	{
		//FString CombinedString = FString::Printf(TEXT("Activate Emitter"));
		//DrawDebugString(GetWorld(), Character->GetActorLocation(), CombinedString, nullptr, FColor::Green, 2.0f, true);
		//FVector HandLocation = Character->GetMesh()->GetSocketLocation(TEXT("Muzzle_02"));
		
		UGameplayStatics::SpawnEmitterAttached(LaunchVFX, Character->GetMesh(), TEXT("Muzzle_02"));
		UGameplayStatics::SpawnEmitterAttached(LaunchVFX, Character->GetMesh(), TEXT("Eyes_Position"));
		UGameplayStatics::SpawnEmitterAttached(LaunchVFX, Character->GetMesh(), TEXT("Muzzle_01"));
	}
}
