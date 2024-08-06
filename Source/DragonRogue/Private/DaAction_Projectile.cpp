// Fill out your copyright notice in the Description page of Project Settings.


#include "DaAction_Projectile.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


UDaAction_Projectile::UDaAction_Projectile()
{
	HandSocketName = "Muzzle_01";
	AttackAnimDelay = 0.2f;
}

void UDaAction_Projectile::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	ACharacter* Character = Cast<ACharacter>(Instigator);
	if (Character)
	{
		// @TODO: Could use animation notify (instead of attack delay timer to time animation)
		Character->PlayAnimMontage(AttackAnim);
		UGameplayStatics::SpawnEmitterAttached(CastingEffect, Character->GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
	
		if (Character->HasAuthority())
		{
			FTimerHandle TimerHandle_AttackDelay;
			FTimerDelegate Delegate;
			Delegate.BindUFunction(this, "AttackDelay_Elapsed", Character);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, AttackAnimDelay, false);
		}
	}
}

void UDaAction_Projectile::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);
}

void UDaAction_Projectile::AttackDelay_Elapsed(ACharacter* InstigatorCharacter)
{
	if (ensure(ProjectileClass) && InstigatorCharacter)
	{
		FVector HandLocation = InstigatorCharacter->GetMesh()->GetSocketLocation(HandSocketName);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = InstigatorCharacter;

		FCollisionShape Shape;
		Shape.SetSphere(20.f);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(InstigatorCharacter);
		
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

		float AttackDistance = 5000.0f;
		FVector Start = InstigatorCharacter->GetPawnViewLocation(); // overrided in DaCharacter
		FRotator ControlRotation = InstigatorCharacter->GetControlRotation();
		FVector End = Start + (ControlRotation.Vector()*AttackDistance);
		FVector ImpactPoint = End;
		
		FHitResult Hit;
		bool bBlockingHit = GetWorld()->SweepSingleByObjectType(Hit, Start, End, FQuat::Identity, ObjectQueryParams, Shape, Params);
		if (bBlockingHit)
		{
			ImpactPoint = Hit.ImpactPoint;
		}
	
		FRotator Rot = FRotationMatrix::MakeFromX(ImpactPoint - HandLocation).Rotator();
		FTransform SpawnTM = FTransform(Rot, HandLocation);
		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);

		// if (CVarDebugPlayerProjectiles.GetValueOnGameThread())
		// {
		// 	float Radius = 30.0f;
		// 	FColor LineColor = bBlockingHit ? FColor::Blue : FColor::Yellow;
		// 	DrawDebugSphere(GetWorld(), ImpactPoint, Radius, 32, LineColor, false, 2.0f);
		// 	DrawDebugLine(GetWorld(), Start, End, LineColor, false, 2.0f, 0, 2.0f);
		// }
	}

	StopAction(InstigatorCharacter);
}