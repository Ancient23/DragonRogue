// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DaAction.generated.h"

class UDaActionComponent;
class UWorld;
/**
 * 
 */
UCLASS(Blueprintable)
class DRAGONROGUE_API UDaAction : public UObject
{
	GENERATED_BODY()

protected:

	UFUNCTION(Blueprintable, Category="Action")
	UDaActionComponent* GetOwningComponent() const;

	UPROPERTY(EditDefaultsOnly, Category="Tags")
	FGameplayTagContainer GrantTags;

	UPROPERTY(EditDefaultsOnly, Category="Tags")
	FGameplayTagContainer BlockedTags;

	
	bool bIsRunning;
	
public:

	UFUNCTION(BlueprintCallable, Category="Action")
	bool IsRunning() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	bool CanStart(AActor* Instigator);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void StopAction(AActor* Instigator);
	
	// Note: FName is hashed (FString is not)
	UPROPERTY(EditDefaultsOnly, Category="Action")
	FName ActionName;

	virtual UWorld* GetWorld() const override;
};
