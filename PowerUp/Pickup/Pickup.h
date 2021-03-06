// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

class USphereComponent;
class UDecalComponent;
class APowerup;

UCLASS()
class SHOOTERUE4_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		UDecalComponent* DecalComponent;

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
		TSubclassOf<APowerup> PowerUpClass;

	APowerup* PowerUpInstance;

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
		float CooldownDuration;

	FTimerHandle TimerHandle_RespawnTimer;

	void Respawn();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		FRotator RotationRate;


};
