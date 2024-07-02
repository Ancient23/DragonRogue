// Fill out your copyright notice in the Description page of Project Settings.


#include "DaCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimPhysicsSolver.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ADaCharacter::ADaCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);
}

// Called when the game starts or when spawned
void ADaCharacter::BeginPlay()
{
	Super::BeginPlay();

	const APlayerController* PlayerController = GetController<APlayerController>();
	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();
	
	Subsystem->AddMappingContext(MoveInputMappingContext, 0);
	
}

void ADaCharacter::Move(const FInputActionInstance& InputInstance)
{
	FRotator ControlRot = GetControlRotation();
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


// Called every frame
void ADaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

	}
}

