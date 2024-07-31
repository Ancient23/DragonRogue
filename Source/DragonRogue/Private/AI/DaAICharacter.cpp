// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/DaAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "DaActionComponent.h"
#include "DaAttributeComponent.h"
#include "DaWorldUserWidget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Util/ColorConstants.h"

// Sets default values
ADaAICharacter::ADaAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
	
	AttributeComp = CreateDefaultSubobject<UDaAttributeComponent>("AttributeComp");

	ActionComp = CreateDefaultSubobject<UDaActionComponent>("ActionComp");
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	TimeToHitParamName = "TimeToHit";
	HitFlashColorParamName = "FlashColor";

	PlayerSeenEmoteTime = 6.0f;
}

void ADaAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &ADaAICharacter::OnPawnSeen);

	AttributeComp->OnHealthChanged.AddDynamic(this, &ADaAICharacter::OnHealthChanged);
}

void ADaAICharacter::OnPawnSeen(APawn* Pawn)
{
	if (IsKnownTargetActor(Pawn))
	{
		return;
	}
	
	SetTargetActor(Pawn); //@TODO: Toggle mode to set player only vs all characters so game designers can choose

	//DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 2.0f, true);

	// Add some cool Widget or emote to NPC to signify when they've spotted the player
	if (PlayerSeenWidget == nullptr)
	{
		PlayerSeenWidget = CreateWidget<UDaWorldUserWidget>(GetWorld(), PlayerSeenWidgetClass);
		if (PlayerSeenWidget)
		{
			PlayerSeenWidget->AttachedActor = this;
			PlayerSeenWidget->AddToViewport();

			FTimerHandle TimerHandle_PlayerSeenElapsed;
			GetWorldTimerManager().SetTimer(TimerHandle_PlayerSeenElapsed, this, &ADaAICharacter::PlayerSeenWidgetTimeExpired, PlayerSeenEmoteTime );
		}
	}
}

void ADaAICharacter::PlayerSeenWidgetTimeExpired()
{
	PlayerSeenWidget->RemoveFromParent();
	PlayerSeenWidget->AttachedActor = nullptr;
	PlayerSeenWidget = nullptr;
}

void ADaAICharacter::OnHealthChanged(AActor* InstigatorActor, UDaAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	if (Delta < 0.0f)
	{
		// Took Damage
		if (InstigatorActor != this)
		{
			SetTargetActor(InstigatorActor);
		}

		if (ActiveHealthBar == nullptr)
		{
			ActiveHealthBar = CreateWidget<UDaWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
			if (ActiveHealthBar)
			{
				ActiveHealthBar->AttachedActor = this;
				ActiveHealthBar->AddToViewport();
			}
		}
		
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
		GetMesh()->SetVectorParameterValueOnMaterials(HitFlashColorParamName, FVector(UE::Geometry::LinearColors::Red3f()));
		
		if (NewHealth <= 0.0f)
		{
			// Dead
		
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

			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetCharacterMovement()->DisableMovement();
			
			// Lifespan
			SetLifeSpan(10.f);
		}
	}
	else if (Delta > 0.0f)
	{
		// Healing
		
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
		GetMesh()->SetVectorParameterValueOnMaterials(HitFlashColorParamName, FVector(UE::Geometry::LinearColors::Green3f()));
	}
}

void ADaAICharacter::SetTargetActor(AActor* NewTarget)
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		AIController->GetBlackboardComponent()->SetValueAsObject("TargetActor", NewTarget);
	}
}

bool ADaAICharacter::IsKnownTargetActor(AActor* Actor)
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		AActor* TargetActor = Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject("TargetActor"));
		if (TargetActor == Actor)
		{
			return true;
		}
	}
	return false;
}


