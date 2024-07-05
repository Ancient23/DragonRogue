// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/Character.h"
#include "DaCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UDaInteractionComponent;

UCLASS()
class DRAGONROGUE_API ADaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADaCharacter();

protected:

	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category="Attack")
	UAnimMontage* AttackAnim;

	FTimerHandle TimerHandle_PrimaryAttack;
	
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere)
	UDaInteractionComponent* InteractionComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	UInputMappingContext* MoveInputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	UInputAction* LookMouseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	UInputAction* LookStickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	UInputAction* PrimaryAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	UInputAction* PrimaryInteractionAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	UInputAction* JumpAction;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Move(const FInputActionInstance& InputInstance);
	void LookMouse(const FInputActionValue& Value);
	void LookStick(const FInputActionValue& InputValue);
	void PrimaryAttack();
	void PrimaryAttack_TimeElapsed();
	void PrimaryInteraction();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
