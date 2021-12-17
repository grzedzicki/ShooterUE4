// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModePlot.h"
#include "DeatchmatchGameMode.generated.h"


/**
 * 
 */
UCLASS()
class SHOOTERUE4_API ADeatchmatchGameMode : public AGameModePlot
{
	GENERATED_BODY()
	
protected:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MP_GameMode")
		void RestartDeadPlayers();

public:

	virtual void PawnKilled(APawn* PawnKilled) override;
};
