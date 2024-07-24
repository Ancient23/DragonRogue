// Fill out your copyright notice in the Description page of Project Settings.


#include "DaCharacter.h"

#include "DaAttributeComponent.h"
#include "DaGameplayFunctionLibrary.h"
#include "DaInteractionComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
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

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<UDaInteractionComponent>("InteractionComp");

	AttributeComp = CreateDefaultSubobject<UDaAttributeComponent>("AttributeComp");
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	bUseControllerRotationYaw = false;

	TimeToHitParamName = "TimeToHit";
	PrimaryAttackHandSocketName = "Muzzle_01";
	HitFlashColorParamName = "FlashColor";
}

void ADaCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ADaCharacter::OnHealthChanged);
}

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

void ADaCharacter::PrimaryAttack()
{	
	//ActionComp->StartActionByName(this, SharedGameplayTags::Action_PrimaryAttack);
	PlayAnimMontage(PrimaryAttackAnim);

	// @TODO: Use animation notify instead of timer
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "Attack_TimeElapsed", PrimaryAttackProjectileClass);
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, Delegate, 0.2f, false);
}

void ADaCharacter::SecondaryAttack()
{
	PlayAnimMontage(SecondaryAttackAnim);

	// @TODO: Use animation notify instead of timer
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "Attack_TimeElapsed", SecondaryAttackProjectileClass);
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, Delegate, 0.2f, false);
}

void ADaCharacter::Dash()
{
	PlayAnimMontage(DashAnim);

	// @TODO: Use animation notify instead of timer
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "Attack_TimeElapsed", DashProjectileClass);
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, Delegate, 0.2f, false);
}

void ADaCharacter::Attack_TimeElapsed(TSubclassOf<AActor> ProjectileClass)
{
	if (ensure(ProjectileClass))
	{
		FVector HandLocation = GetMesh()->GetSocketLocation(PrimaryAttackHandSocketName);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = this;

		FCollisionShape Shape;
		Shape.SetSphere(20.f);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

		float AttackDistance = 5000.0f;
		FVector Start = CameraComp->GetComponentLocation();
		//FRotator CameraRotation = CameraComp->GetComponentRotation();
		FRotator ControlRotation = GetControlRotation();
		FVector End = Start + (ControlRotation.Vector()*AttackDistance);
		FVector ImpactPoint = End;
		
		FHitResult Hit;
		bool bBlockingHit = GetWorld()->SweepSingleByObjectType(Hit, Start, End, FQuat::Identity, ObjectQueryParams, Shape, Params);
		if (bBlockingHit)
		{
			ImpactPoint = Hit.ImpactPoint;
		}
	
		FRotator Rot = UKismetMathLibrary::MakeRotFromX(ImpactPoint - HandLocation);
		FTransform SpawnTM = FTransform(Rot, HandLocation);
		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);

		if (CVarDebugPlayerProjectiles.GetValueOnGameThread())
		{
			float Radius = 30.0f;
			FColor LineColor = bBlockingHit ? FColor::Blue : FColor::Yellow;
			DrawDebugSphere(GetWorld(), ImpactPoint, Radius, 32, LineColor, false, 2.0f);
			DrawDebugLine(GetWorld(), Start, End, LineColor, false, 2.0f, 0, 2.0f);
		}
	}
}


void ADaCharacter::PrimaryInteraction()
{
	if (InteractionComp) InteractionComp->PrimaryInteract();
}

void ADaCharacter::OnHealthChanged(AActor* InstigatorActor, UDaAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	if (NewHealth <= 0.0f && Delta < 0.0f)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);
	}
	else if (Delta < 0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
		GetMesh()->SetVectorParameterValueOnMaterials(HitFlashColorParamName, FVector(UE::Geometry::LinearColors::Red3f()));
	} else if (Delta > 0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
		GetMesh()->SetVectorParameterValueOnMaterials(HitFlashColorParamName, FVector(UE::Geometry::LinearColors::Green3f()));
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
		
	}
}

void ADaCharacter::HealSelf(float Amount /* = 100 */)
{
	UDaGameplayFunctionLibrary::ApplyHealing(this, this, Amount);
}

