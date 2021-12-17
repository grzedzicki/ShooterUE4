// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MP_PlayerStatus.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERUE4_API AMP_PlayerStatus : public APlayerState
{
	GENERATED_BODY()
	
		UFUNCTION(BlueprintCallable, Category = "PlayerState")
			void AddScore(float ScoreDelta);
};
