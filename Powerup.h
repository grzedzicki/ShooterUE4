// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Powerup.generated.h"

UCLASS()
class SHOOTERUE4_API APowerup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APowerup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Time between powerup ticks */
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
		float PowerupInterval;

	// 
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
		int32 TotalNrOfTicks;

	FTimerHandle TimerHandle_PowerupTick;

	// Ilość Ticków
	int32 TicksProcessed;

	UFUNCTION()
		void OnTickPowerup();

	UPROPERTY(ReplicatedUsing = OnRep_PowerupActive)
		bool bIsPowerupActive;

	UFUNCTION()
		void OnRep_PowerupActive();


	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
		void OnPowerupStateChanged(bool bNewIsActive);

public:	

	virtual void Tick(float DeltaTime) override;


	void ActivatePowerup(AActor* ActiveFor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
		void OnActivated(AActor* ActiveFor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
		void OnPowerupTicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
		void OnExpired();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		FRotator RotationRate;
};
