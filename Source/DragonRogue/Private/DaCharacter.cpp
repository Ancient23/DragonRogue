// Fill out your copyright notice in the Description page of Project Settings.


#include "DaCharacter.h"

#include "DaInteractionComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimPhysicsSolver.h"
#include "Camera/CameraComponent.h"
#include "DragonRogue/DragonRogue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/BlueprintTypeConversions.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ADaCharacter::ADaCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<UDaInteractionComponent>("InteractionComp");
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	bUseControllerRotationYaw = false;
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

void ADaCharacter::PrimaryAttack()
{	
	//ActionComp->StartActionByName(this, SharedGameplayTags::Action_PrimaryAttack);
	PlayAnimMontage(AttackAnim);

	// @TODO: Use animation notify instead of timer
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ADaCharacter::PrimaryAttack_TimeElapsed, 0.2f);
}

void ADaCharacter::PrimaryAttack_TimeElapsed()
{
	FVector HandLocation = GetMesh()->GetSocketLocation(TEXT("Muzzle_01"));

	// Use Yaw from Character but pitch from camera
	//FRotator SpawnRot = GetActorRotation();
	//float CameraPitch = GetControlRotation().Pitch;

	//LOG("CameraPitch: %f", CameraPitch);
	// Adjust fire up slightly if camera is looking close to horizon, or keep aim up a bit if below horizon
	//if (CameraPitch<30.0f)
	//	CameraPitch = CameraPitch + (CameraPitch*0.3f);
	//else if (CameraPitch>270.0f)
	//	CameraPitch = CameraPitch + (359.0f - CameraPitch)*0.6f; 
	
	//SpawnRot.Pitch = CameraPitch;
	//FTransform SpawnTM = FTransform(SpawnRot, HandLocation);
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	
	FVector CameraLocation = CameraComp->GetComponentLocation();
	FRotator CameraRotation = CameraComp->GetComponentRotation();
	FVector End = CameraLocation + (CameraRotation.Vector()*5000);
	
	float Radius = 30.0f;
	FHitResult Hit;
	bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, CameraLocation, End, ObjectQueryParams);
	FColor LineColor = bBlockingHit ? FColor::Blue : FColor::Yellow;
	if (AActor* HitActor = Hit.GetActor())
	{
		DrawDebugSphere(GetWorld(), Hit.ImpactPoint, Radius, 32, LineColor, false, 2.0f);

		FRotator Rot = UKismetMathLibrary::MakeRotFromX(Hit.ImpactPoint - HandLocation);
		FTransform SpawnTM = FTransform(Rot, HandLocation);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = this;
	
		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
	} else
	{
		// nothing hit, use trace end as desired target

		DrawDebugSphere(GetWorld(), End, Radius, 32, LineColor, false, 2.0f);

		FRotator Rot = UKismetMathLibrary::MakeRotFromX(End - HandLocation);
		FTransform SpawnTM = FTransform(Rot, HandLocation);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = this;
	
		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
	}
	//DrawDebugLine(GetWorld(), CameraLocation, End, LineColor, false, 2.0f, 0, 2.0f);
	
}


void ADaCharacter::PrimaryInteraction()
{
	if (InteractionComp) InteractionComp->PrimaryInteract();
}

// Called every frame
void ADaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

		// Primary Attack/Ineraction
		EnhancedInputComponent->BindAction(PrimaryAttackAction, ETriggerEvent::Triggered, this, &ADaCharacter::PrimaryAttack);
		EnhancedInputComponent->BindAction(PrimaryInteractionAction, ETriggerEvent::Triggered, this, &ADaCharacter::PrimaryInteraction);

		// Jump
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);

		
	}
}

