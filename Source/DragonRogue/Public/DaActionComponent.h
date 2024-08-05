// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "DaActionComponent.generated.h"


class UDaAction;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DRAGONROGUE_API UDaActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Actions")
	static UDaActionComponent* GetActions(AActor* FromActor);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tags")
	FGameplayTagContainer ActiveGameplayTags;
	
	UFUNCTION(BlueprintCallable, Category="Actions")
	void AddAction(AActor* Instigator, TSubclassOf<UDaAction> ActionClass);

	UFUNCTION(BlueprintCallable, Category="Actions")
	void RemoveAction(UDaAction* ActionToRemove);
	
	UFUNCTION(BlueprintCallable, Category="Actions")
	bool StartActionByName(AActor* Instigator, FName ActionName);

	UFUNCTION(BlueprintCallable, Category="Actions")
	bool StopActionByName(AActor* Instigator, FName ActionName);

	UFUNCTION(BlueprintCallable, Category="Actions")
	bool ContainsActionWithName(FName ActionName);
	
	UFUNCTION(BlueprintCallable, Category="Actions")
	float GetActionCostByName(FName ActionName);
	
	UDaActionComponent();

protected:

	UFUNCTION(Server, Reliable)
	void ServerStartAction(AActor* Instigator, FName ActionName);

	UPROPERTY(EditAnywhere, Category="Actions")
	TArray<TSubclassOf<UDaAction>> DefaultActions;
	
	UPROPERTY()
	TArray<TObjectPtr<UDaAction>> Actions;
	
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool bHasBegunPlay = false;
};
