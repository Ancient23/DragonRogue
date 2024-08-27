// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DaAnimInstance.generated.h"

class UDaActionComponent;
/**
 * 
 */
UCLASS()
class DRAGONROGUE_API UDaAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:

	// Pawn stunned based on gameplay tags
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Animation")
	bool bIsStunned;

	UPROPERTY(BlueprintReadOnly, Category="Animation")
	TObjectPtr<UDaActionComponent> ActionComp;
	
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
