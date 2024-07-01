// Fill out your copyright notice in the Description page of Project Settings.


#include "ADGCharacter.h"

// Sets default values
AADGCharacter::AADGCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AADGCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AADGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AADGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

