// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MP_GameMode.generated.h"


enum class EWaveStatus : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController);

/**
 * 
 */
UCLASS()
class SHOOTERUE4_API AMP_GameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:


	void SpawnBotTimerElapsed();

	void StartWave();
	
	void EndWave();

	void PrepareForNextWave();

	void CheckWaveState();

	void CheckAnyPlayerAlive();

	void GameOver();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MP_GameMode")
		void GameOverForBP();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MP_GameMode")
		void StartWaveForBP();

	UFUNCTION(BlueprintImplementableEvent, Category = "MP_GameMode")
		void SpawnNewBot();

	void SetWaveStatus(EWaveStatus NewState);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MP_GameMode")
	void RestartDeadPlayers();

	FTimerHandle TimerHandle_BotSpawner;

	FTimerHandle TimerHandle_NextWaveStart;

	int32 NrOfBotsToSpawn;

	UPROPERTY(BlueprintReadWrite, Category = "MP_GameMode")
	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly, Category = "MP_GameMode")
		float TimeBetweenWaves;

public:

	AMP_GameMode();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintAssignable, Category = "GameMode")
		FOnActorKilled OnActorKilled;
};
