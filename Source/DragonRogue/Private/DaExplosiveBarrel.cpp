// Fill out your copyright notice in the Description page of Project Settings.


#include "DaExplosiveBarrel.h"

#include "AssetTypeCategories.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
ADaExplosiveBarrel::ADaExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComp");
	RootComponent = StaticMeshComp;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BarrelAsset(TEXT("/Game/ExampleContent/Meshes/SM_RedBarrel.SM_RedBarrel"));
	static ConstructorHelpers::FObjectFinder<UMaterial> BarrelMat(TEXT("/Engine/MapTemplates/Materials/BasicAsset01.BasicAsset01"));
	if (BarrelAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(BarrelAsset.Object);
	}
	if (BarrelMat.Succeeded())
	{
		StaticMeshComp->SetMaterial(0, BarrelMat.Object);
	}

	StaticMeshComp->SetSimulatePhysics(true);
	StaticMeshComp->SetCollisionProfileName("PhysicsActor");

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>("RadialForceComp");
	RadialForceComp->Radius = 500.f;
	RadialForceComp->ImpulseStrength = 800.0f;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->SetupAttachment(StaticMeshComp);
}

void ADaExplosiveBarrel::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor!=nullptr)&&(OtherActor!=this)&&(OtherComp!=nullptr))
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Hit: %s"), *OtherActor->GetName()));
		RadialForceComp->FireImpulse();
	}
}

// runs before begin play but after init
void ADaExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StaticMeshComp->OnComponentHit.AddDynamic(this, &ADaExplosiveBarrel::OnComponentHit);
}


// Called when the game starts or when spawned
void ADaExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADaExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

