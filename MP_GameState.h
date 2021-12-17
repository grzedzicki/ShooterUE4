// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MP_GameState.generated.h"

UENUM(BlueprintType)
enum class EWaveStatus : uint8
{
	PrepareNextWave,
	WaveInProgress,
	WaitingToComplete,
	WaveComplete,
	GameOver
};

/**
 * 
 */
UCLASS()
class SHOOTERUE4_API AMP_GameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	void SetWaveStatus(EWaveStatus NewState);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameState")
	void SetWaveNumber(int Number);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameState")
		void GameOverState();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameState")
		void UpdateClientWave();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameState", Replicated)
		int32 WaveNumber;

protected:
	UFUNCTION()
		void OnRep_WaveStatus(EWaveStatus OldState);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
		void WaveStatusChanged(EWaveStatus NewState, EWaveStatus OldState);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WaveStatus, Category = "GameState")
		EWaveStatus WaveStatus;
};
