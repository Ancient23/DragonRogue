// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DaAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAttributeChanged, AActor*, InstigatorActor, UDaAttributeComponent*, OwningComp, float, NewAttribValue, float, Delta);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRageChanged, UDaAttributeComponent*, OwningComp, float, NewRage, float, Delta);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DRAGONROGUE_API UDaAttributeComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category="Attributes")
	static UDaAttributeComponent* GetAttributes(AActor* FromActor);

	UFUNCTION(BlueprintCallable, Category="Attributes", meta = (DisplayName = "IsAlive"))
	static bool IsActorAlive(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category="Attributes", meta = (DisplayName = "HasLowHealth"))
	static bool HasLowHealth(AActor* Actor);
	
	// Sets default values for this actor's properties
	UDaAttributeComponent();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category="Attributes")
	float Rage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category="Attributes")
	float RageMax;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category="Attributes")
	float RageMultiplier;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category="Attributes")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category="Attributes")
	float HealthMax;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category="Attributes")
	float LowHealthThreshold;

	// useful for broadcasting but more effects
	// @FIXME: Mark As Unreliable and use bAsAlive as state
	// Unreliable also considers relavancy based on distance for example
	UFUNCTION(NetMulticast, Reliable)
	void MulticastHealthChanged(AActor* InstigatorActor, float NewHealth, float Delta);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	//@TODO: Stamina, Strength
public:

	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool Kill(AActor* InstigatorActor);
	
	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool LowHealth() const;

	UFUNCTION(BlueprintCallable, Category="Attributes")
	float GetHealthMax() const;

	UFUNCTION(BlueprintCallable, Category="Attributes")
	float GetHealth() const;
	
	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnAttributeChanged OnHealthChanged;
	
	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool ApplyHealthChange(AActor* InstigatorActor, float Delta);

	UFUNCTION(BlueprintCallable, Category="Attributes")
	float GetRageMax() const;

	UFUNCTION(BlueprintCallable, Category="Attributes")
	float GetRage() const;

	UFUNCTION(BlueprintCallable, Category="Attributes")
	float AddRage(float Amount);

	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool UseRage(float Amount);
	
	UFUNCTION(BlueprintCallable, Category="Attributes")
	float SetRageToMax();
	
	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnAttributeChanged OnRageChanged;
};
