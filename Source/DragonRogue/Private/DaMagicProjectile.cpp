// Fill out your copyright notice in the Description page of Project Settings.


#include "DaMagicProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ADaMagicProjectile::ADaMagicProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MovementComp->InitialSpeed = 1000.0f;

}

// Called when the game starts or when spawned
void ADaMagicProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADaMagicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

