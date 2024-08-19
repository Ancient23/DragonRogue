// Fill out your copyright notice in the Description page of Project Settings.


#include "DaCharacter.h"

#include "DaActionComponent.h"
#include "DaAttributeComponent.h"
#include "DaGameplayFunctionLibrary.h"
#include "DaInteractionComponent.h"
#include "DaPlayerController.h"
#include "DaPlayerState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Util/ColorConstants.h"

static TAutoConsoleVariable<bool> CVarDebugPlayerArrows(TEXT("da.DrawPlayerOrientation"), false, TEXT("Enable Debug Draw of Player Actor and Pawn Orientation arrows"), ECVF_Cheat);
static TAutoConsoleVariable<bool> CVarDebugPlayerProjectiles(TEXT("da.DrawPlayerProjectiles"), false, TEXT("Enable Debug Draw of Player Spawned DaProjectile children"), ECVF_Cheat);

// Sets default values
ADaCharacter::ADaCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	// We control the rotation of spring arm with pawn control rotation already, this disables a subtle side effect
	// where rotating our CapsuleComponent (eg. caused by bOrientRotationToMovement in Character Movement) will rotate our spring arm until it self corrects later in the update
	// This may cause unwanted effects when using CameraLocation during Tick as it may be slightly offset from our final camera position.
	SpringArmComp->SetUsingAbsoluteRotation(true);
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<UDaInteractionComponent>("InteractionComp");

	AttributeComp = CreateDefaultSubobject<UDaAttributeComponent>("AttributeComp");

	ActionComp = CreateDefaultSubobject<UDaActionComponent>("ActionComp");
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	// Skip performing overlap queries on the Physics Asset after animation (7 queries in case of our Gideon mesh)
	GetMesh()->bUpdateOverlapsOnAnimationFinalize = false;

	// Enabled on mesh to react to incoming projectiles
	GetMesh()->SetGenerateOverlapEvents(true);
	// Disable on capsule collision to avoid double-dipping and receiving 2 overlaps when entering trigger zones etc.
	// Once from the mesh, and 2nd time from capsule
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	
	TimeToHitParamName = "TimeToHit";
	HitFlashColorParamName = "FlashColor";
}

void ADaCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ADaCharacter::OnHealthChanged);
}

// overriding Pawn method for 3rd person perspective
FVector ADaCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}

// Called when the game starts or when spawned
void ADaCharacter::BeginPlay()
{
	Super::BeginPlay();

	const APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	
		UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		check(Subsystem);

		Subsystem->ClearAllMappings();
	
		Subsystem->AddMappingContext(MoveInputMappingContext, 0);
	}
	
}

void ADaCharacter::Move(const FInputActionInstance& InputInstance)
{
	FRotator ControlRot = GetControlRotation();

	// only need yaw
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	// Combined Input VaLue from WASD or Gamestick -> convert to vector
	const FVector2d AxisValue = InputInstance.GetValue().Get<FVector2d>();

	// Forward/Back
	AddMovementInput(ControlRot.Vector(), AxisValue.Y);

	// Strafe
	const FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);
	AddMovementInput(RightVector, AxisValue.X);
}

void ADaCharacter::LookMouse(const FInputActionValue& InputValue)
{
	const FVector2d Value = InputValue.Get<FVector2d>();

	AddControllerYawInput(Value.X);
	AddControllerPitchInput(Value.Y);
}

void ADaCharacter::LookStick(const FInputActionValue& InputValue)
{
	FVector2d Value = InputValue.Get<FVector2d>();

	// track if negative
	bool XNegative = Value.X < 0.0f;
	bool YNegative = Value.Y < 0.0f;
	
	// nonlinear makes aim easier
	Value = Value*Value;

	XNegative?Value.X*=-1.f:Value.X*=1.f;
	YNegative?Value.Y*=-1.f:Value.Y*=1.f;

	// @TODO: futher modify with sensativity settings
	static const float LookYawRate = 100.0f;
	static const float LookPitchRate = 50.0f;
	
	// @todo: increase accelerate longer input is > 0 on axis
	const float deltaSeconds = GetWorld()->GetDeltaSeconds();
	AddControllerYawInput(Value.X*LookYawRate*deltaSeconds);
	AddControllerPitchInput(Value.Y*LookPitchRate*deltaSeconds);
}

void ADaCharacter::SprintStart()
{
	ActionComp->StartActionByName(this, "Sprint");
}

void ADaCharacter::SprintStop()
{
	ActionComp->StopActionByName(this, "Sprint");
}

void ADaCharacter::PrimaryAttack()
{	
	ActionComp->StartActionByName(this, "PrimaryAttack");
}

void ADaCharacter::SecondaryAttack()
{
	float RageCost = ActionComp->GetActionCostByName("SecondaryAttack");
	if (RageCost > 0.0f && AttributeComp->GetRage() >= RageCost)
	{
		if (ActionComp->StartActionByName(this, "SecondaryAttack"))
		{
			AttributeComp->UseRage(RageCost);
		}
	} else if (RageCost == 0.0f)
	{
		ActionComp->StartActionByName(this, "SecondaryAttack");
	}
}

void ADaCharacter::Dash()
{
	ActionComp->StartActionByName(this, "Dash");
}

void ADaCharacter::PrimaryInteraction()
{
	if (InteractionComp) InteractionComp->PrimaryInteract();
}

void ADaCharacter::OnHealthChanged(AActor* InstigatorActor, UDaAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	if (Delta < 0.0f)
	{
		// Damage Effects
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
		GetMesh()->SetVectorParameterValueOnMaterials(HitFlashColorParamName, FVector(UE::Geometry::LinearColors::Red3f()));
		
		AttributeComp->AddRage(FMath::Abs(Delta));
	}
	else if (Delta > 0.0f)
	{
		// Healing Effects
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
		GetMesh()->SetVectorParameterValueOnMaterials(HitFlashColorParamName, FVector(UE::Geometry::LinearColors::Green3f()));
	}

	if (NewHealth <= 0.0f && Delta < 0.0f)
	{
		// Death
		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);

		SetLifeSpan(5.0f);
	}
}

// Called every frame
void ADaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CVarDebugPlayerArrows.GetValueOnGameThread())
	{
		// -- Rotation Visualization -- //
		const float DrawScale = 100.0f;
		const float Thickness = 5.0f;

		FVector LineStart = GetActorLocation();
		// Offset to the right of pawn
		LineStart += GetActorRightVector() * 100.0f;
		// Set line end in direction of the actor's forward
		FVector ActorDirection_LineEnd = LineStart + (GetActorForwardVector() * 100.0f);
		// Draw Actor's Direction
		DrawDebugDirectionalArrow(GetWorld(), LineStart, ActorDirection_LineEnd, DrawScale, FColor::Yellow, false, 0.0f, 0, Thickness);

		FVector ControllerDirection_LineEnd = LineStart + (GetControlRotation().Vector() * 100.0f);
		// Draw 'Controller' Rotation ('PlayerController' that 'possessed' this character)
		DrawDebugDirectionalArrow(GetWorld(), LineStart, ControllerDirection_LineEnd, DrawScale, FColor::Green, false, 0.0f, 0, Thickness);
	}
}

// Called to bind functionality to input
void ADaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADaCharacter::Move);

		// Look
		EnhancedInputComponent->BindAction(LookMouseAction, ETriggerEvent::Triggered, this, &ADaCharacter::LookMouse);
		EnhancedInputComponent->BindAction(LookStickAction, ETriggerEvent::Triggered, this, &ADaCharacter::LookStick);

		// Attack/Interaction
		EnhancedInputComponent->BindAction(PrimaryAttackAction, ETriggerEvent::Triggered, this, &ADaCharacter::PrimaryAttack);
		EnhancedInputComponent->BindAction(SecondaryAttackAction, ETriggerEvent::Triggered, this, &ADaCharacter::SecondaryAttack);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &ADaCharacter::Dash);
		EnhancedInputComponent->BindAction(PrimaryInteractionAction, ETriggerEvent::Triggered, this, &ADaCharacter::PrimaryInteraction);

		// Jump
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);

		// Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ADaCharacter::SprintStart);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ADaCharacter::SprintStop);

		// Toggle Pause Menu
		EnhancedInputComponent->BindAction(PausseMenuAction, ETriggerEvent::Started, Cast<ADaPlayerController>(this->Controller.Get()), &ADaPlayerController::TogglePauseMenu);
	}
}

bool ADaCharacter::IsRageActionAvailable() const
{
	if (!ActionComp->bHasBegunPlay)
	{
		return false;
	}
	
	float RageCost = ActionComp->GetActionCostByName("SecondaryAttack");
	if (AttributeComp->GetRage() >= RageCost)
	{
		return true;
	}

	return false;
}

void ADaCharacter::HealSelf(float Amount /* = 100 */)
{
	UDaGameplayFunctionLibrary::ApplyHealing(this, this, Amount);
}

void ADaCharacter::RageBoostMax()
{
	AttributeComp->SetRageToMax();
}