// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DaGameModeBase.generated.h"

namespace EEnvQueryStatus
{
	enum Type : int;
}

class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
class UCurveFloat;
class AController;

/**
 * 
 */
UCLASS()
class DRAGONROGUE_API ADaGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category="Credits")
	float CreditsPerKill;
	
	UPROPERTY(EditDefaultsOnly, Category="AI")
	TSubclassOf<AActor> MinionClass;
	
	UPROPERTY(EditDefaultsOnly, Category="AI")
	UEnvQuery* SpawnBotQuery;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	UCurveFloat* DifficultyCurve;
	
	FTimerHandle TimerHandle_SpawnBots;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	float SpawnTimerInterval;

	UFUNCTION()
	void SpawnBotTimerElapsed();

	UFUNCTION()
	void OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);
	
public:

	virtual void OnActorKilled(AActor* VictimActor, AActor* KillerActor);
	
	ADaGameModeBase();
	
	virtual void StartPlay() override;

	UFUNCTION(Exec)
	void KillAll();
};
