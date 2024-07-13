// Fill out your copyright notice in the Description page of Project Settings.


#include "DaMagicProjectile.h"

#include "DaAttributeComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ADaMagicProjectile::ADaMagicProjectile()
{
	MovementComp->InitialSpeed = 1000.0f;
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ADaMagicProjectile::OnActorOverlap);
}

void ADaMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor && OtherActor != GetInstigator())
	{
		UDaAttributeComponent* AttributeComp = Cast<UDaAttributeComponent>(OtherActor->GetComponentByClass(UDaAttributeComponent::StaticClass()));
		if (AttributeComp)
		{
			AttributeComp->ApplyHealthChange(-20.0f);
		}

		Explode();
	}
}


