// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DaWorldUserWidget.generated.h"

class USizeBox;
/**
 * 
 */
UCLASS()
class DRAGONROGUE_API UDaWorldUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> ParentSizeBox;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public: 
	UPROPERTY(BlueprintReadWrite, Category="UI", meta = (ExposeOnSpawn=true))
	TObjectPtr<AActor> AttachedActor;

	UPROPERTY(EditAnywhere, Category="UI")
	FVector WorldOffset;
};
