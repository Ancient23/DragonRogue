// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/GameModeBase.h"
#include "DaGameModeBase.generated.h"

class ADaPickupItem;

namespace EEnvQueryStatus
{
	enum Type : int;
}

class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
class UCurveFloat;
class AController;
class UDaSaveGame;

/**
 * 
 */
UCLASS()
class DRAGONROGUE_API ADaGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:

	FString SlotName;
	
	UPROPERTY()
	UDaSaveGame* CurrentSaveGame;
	
	UPROPERTY(EditAnywhere, Category="Credits")
	int32 CreditsPerKill;

	UPROPERTY(EditDefaultsOnly, Category="Pickups")
	UEnvQuery* SpawnItemQuery;

	UPROPERTY(EditDefaultsOnly, Category="Pickups")
	TMap<FGameplayTag, TSubclassOf<ADaPickupItem>> ItemClasses;
	
	UPROPERTY(EditDefaultsOnly, Category="AI")
	TSubclassOf<AActor> MinionClass;
	
	UPROPERTY(EditDefaultsOnly, Category="AI")
	UEnvQuery* SpawnBotQuery;
	
	UPROPERTY(EditDefaultsOnly, Category="AI")
	UCurveFloat* DifficultyCurve;

	// Values > 0 will override DifficultyCurve. Note: Leave at 0 to use Difficulty Curve
	UPROPERTY(EditDefaultsOnly, Category="AI")
	uint32 MaxBotsOverride;
	
	FTimerHandle TimerHandle_SpawnBots;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	float SpawnTimerInterval;

	UFUNCTION()
	void SpawnBotTimerElapsed();

	UFUNCTION()
	void OnSpawnBotQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void OnSpawnItemQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	
	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);
	
public:

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	
	virtual void OnActorKilled(AActor* VictimActor, AActor* KillerActor);
	
	ADaGameModeBase();
	
	virtual void StartPlay() override;

	UFUNCTION(Exec)
	void KillAll();

	UFUNCTION(BlueprintCallable, Category="SaveGame")
	void WriteSaveGame();

	void LoadSaveGame();

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
};
