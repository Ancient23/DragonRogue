// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/DaAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "DaAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"

// Sets default values
ADaAICharacter::ADaAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
	
	AttributeComp = CreateDefaultSubobject<UDaAttributeComponent>("AttributeComp");

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ADaAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &ADaAICharacter::OnPawnSeen);

	AttributeComp->OnHealthChanged.AddDynamic(this, &ADaAICharacter::OnHealthChanged);
}

void ADaAICharacter::OnPawnSeen(APawn* Pawn)
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		UBlackboardComponent* BBComp = AIController->GetBlackboardComponent();
		BBComp->SetValueAsObject("TargetActor", Pawn);

		DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 2.0f, true);
	}
}

void ADaAICharacter::OnHealthChanged(AActor* InstigatorActor, UDaAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	if (NewHealth <= 0.0f && Delta < 0.0f)
	{
		// stop BT
		AAIController* AIController = Cast<AAIController>(GetController());
		if (AIController)
		{
			AIController->GetBrainComponent()->StopLogic("Killed");
		}
		
		// Ragdoll
		// could also use anim bp, but will code it here.
		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetCollisionProfileName("Ragdoll");

		// Lifespan
		SetLifeSpan(10.f);
	}
	else if (Delta < 0.0f)
	{
		//GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
		//GetMesh()->SetVectorParameterValueOnMaterials(HitFlashColorParamName, FVector(UE::Geometry::LinearColors::Red3f()));
	} else if (Delta > 0.0f)
	{
		//GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
		//GetMesh()->SetVectorParameterValueOnMaterials(HitFlashColorParamName, FVector(UE::Geometry::LinearColors::Green3f()));
	}
}


