// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DaInteractionComponent.generated.h"


class UDaWorldUserWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DRAGONROGUE_API UDaInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void PrimaryInteract();
	
	// Sets default values for this component's properties
	UDaInteractionComponent();

protected:

	void FindBestInteractable();
	
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	AActor* FocusedActor;

	UPROPERTY(EditDefaultsOnly, Category="Trace")
	float TraceDistance;

	UPROPERTY(EditDefaultsOnly, Category="Trace")
	float TraceRadius;

	UPROPERTY(EditDefaultsOnly, Category="Trace")
	TEnumAsByte<ECollisionChannel> CollisionChannel;
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UDaWorldUserWidget> DefaultWidgetClass;

	UPROPERTY()
	UDaWorldUserWidget* DefaultWidgetInstance;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
