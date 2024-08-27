// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameModeBase.h"
#include "DaGameModeBase.generated.h"

class UDaMonsterData;
class ADaPickupItem;
class UEnvQuery;
class UCurveFloat;
class AController;

USTRUCT(BlueprintType)
struct FMonsterInfoRow : public FTableRowBase
{
	GENERATED_BODY()

public:

	FMonsterInfoRow()
	{
		Weight = 1.0f;
		SpawnCost = 5.0f;
		KillReward = 20.0f;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPrimaryAssetId MonsterId;
	//UDaMonsterData* MonsterData;

	// Relative chance to pick this monster
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight;

	// Points require by gamemode to spawn
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnCost;

	// Credits awarded to killer
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KillReward;
};

/**
 * 
 */
UCLASS()
class DRAGONROGUE_API ADaGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditAnywhere, Category="Credits")
	int32 CreditsPerKill;

	UPROPERTY(EditDefaultsOnly, Category="Pickups")
	TObjectPtr<UEnvQuery> SpawnItemQuery;

	UPROPERTY(EditDefaultsOnly, Category="Pickups")
	TMap<FGameplayTag, TSubclassOf<ADaPickupItem>> ItemClasses;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UDataTable> MonsterTable;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ruleset")
	bool bAutoRespawnPlayer;
	
	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UEnvQuery> SpawnBotQuery;
	
	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UCurveFloat> DifficultyCurve;

	// Values > 0 will override DifficultyCurve. Note: Leave at 0 to use Difficulty Curve
	UPROPERTY(EditDefaultsOnly, Category="AI")
	uint32 MaxBotsOverride;
	
	/* Amount available to start spawning some bots immediately */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	int32 InitialSpawnCredit;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	bool bAutoStartBotSpawning;
	
	UPROPERTY(EditDefaultsOnly, Category="AI")
	float SpawnTimerInterval;

	UFUNCTION()
	void SpawnBotTimerElapsed();

	void OnSpawnBotQueryCompleted(TSharedPtr<FEnvQueryResult> Result);

	void OnSpawnItemQueryCompleted(TSharedPtr<FEnvQueryResult> Result);
	
	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);

	void OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation);

	FTimerHandle TimerHandle_SpawnBots;

public:

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	
	virtual void OnActorKilled(AActor* VictimActor, AActor* KillerActor);
	
	ADaGameModeBase();
	
	virtual void StartPlay() override;

	UFUNCTION(BlueprintCallable)
	void StartSpawningBots();
	
	UFUNCTION(Exec)
	void KillAll();

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
};
