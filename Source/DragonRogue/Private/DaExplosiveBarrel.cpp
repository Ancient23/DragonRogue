// Fill out your copyright notice in the Description page of Project Settings.


#include "DaExplosiveBarrel.h"
#include "DaGameplayFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include "PhysicsEngine/RadialForceComponent.h"

static TAutoConsoleVariable<bool> CVarDebugExplosiveItem(TEXT("da.DebugExplosiveItem"), false, TEXT("Enable Debug Drawing for Explosive Level Items"), ECVF_Cheat);

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

	DamageAmount = 50.0f;

	bReplicates = true;
}

void ADaExplosiveBarrel::OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	bool bDebug = CVarDebugExplosiveItem.GetValueOnGameThread();
	
	if ((OtherActor!=nullptr)&&(OtherActor!=this)&&(OtherComp!=nullptr))
	{
		if (bDebug && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Hit: %s"), *OtherActor->GetName()));
		}
		
		RadialForceComp->FireImpulse();

		// if its a player with an attribute component hit it hard
		bool bDamaged = UDaGameplayFunctionLibrary::ApplyDamage(this, OtherActor, DamageAmount);

		if (bDebug)
		{
			FString CombinedString = FString::Printf(TEXT("Hit at Location: %s, Damaged: %hs"), *Hit.ImpactPoint.ToString(), bDamaged?"true":"false");
			DrawDebugString(GetWorld(), Hit.ImpactPoint, CombinedString, nullptr, FColor::Green, 2.0f, true);
		}
	}
}

// runs before begin play but after init
void ADaExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StaticMeshComp->OnComponentHit.AddDynamic(this, &ADaExplosiveBarrel::OnActorHit);
}

void ADaExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADaExplosiveBarrel, DamageAmount);
}

