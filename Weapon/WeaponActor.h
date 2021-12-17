// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponActor.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;

// Struktura do obługi uderzeń w różne powierzchnie
USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

public:

	UPROPERTY()
		TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
		FVector_NetQuantize TraceTo;

};

UCLASS()
class SHOOTERUE4_API AWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Ustawanie domyślnych wartości dla tego aktora
	AWeaponActor();



protected:
	// Funkcja wywoływana przy początku gry, lub przy odrodzeniu
	virtual void BeginPlay() override;

	// Mesh broni
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* MeshComponent;

	// Trzęsienie kamery podczas strzelania
	UPROPERTY(EditDefaultsOnly, Category = "WeaponActor")
		TSubclassOf<UCameraShakeBase> FireCamShake;

	// Efekt strzału z broni
	void PlayFireEffects(FVector TraceEndPoint);

	// Odtworzenie efektu uderzenia pocisku w powierzchnię
	void PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	// Rodzaj obrażenia
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponActor")
		TSubclassOf<UDamageType> DamageType;

	// Definicja umieszczenia lufy broni
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "WeaponActor")
		FName MuzzleSocket;

	//
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "WeaponActor")
		FName TracerTargetName;

	// Efekt wystrzału z broni
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponActor")
		UParticleSystem* MuzzleFlash;

	// Domyślny efekt uderzenia pocisku w powierzchnię
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponActor")
		UParticleSystem* DefaultImpactEffect;

	// Efekt uderzenia pocisku w ciało przeciwnika/gracza
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponActor")
		UParticleSystem* FleshImpactEffect;

	// Efekt smugi pocisku
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponActor")
		UParticleSystem* TracerEffect;

	// Podstawowa wartość obrażeń od pocisku
	UPROPERTY(EditDefaultsOnly, Category = "WeaponActor")
		float BaseDamage;

	

	// Replikacja strzału u innych klientów
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFire();

	UPROPERTY(EditAnywhere)
		USoundBase* MuzzleSound;

	// Timer między strzałami
	FTimerHandle TimerHandle_TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly, Category = "WeaponActor", meta = (ClampMin=0.0f))
		float BulletSpread;

	float LastFire;

	// Szybkostrzelność broni
	UPROPERTY(EditDefaultsOnly, Category = "WeaponActor")
		float RateOfFire;

	// Czas między oddanymi strzałami
	float TimeBetweenShots;

	// Replikacja co zostało trafione
	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
		FHitScanTrace HitScanTrace;

	// Sprawdzenie co zostało trafione
	UFUNCTION()
		void OnRep_HitScanTrace();

	// Ilość naboi w magazynku
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", Replicated)
		int32 AmmoClip;

	// Ilość zapasowych naboi
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
		int32 AmmoPool;

public:	
	// Funkcja wywoływana w każdej klatce
	virtual void Tick(float DeltaTime) override;

	//UFUNCTION(BlueprintCallable, Category = "WeaponActor")
		void StartFire();

	//UFUNCTION(BlueprintCallable, Category = "WeaponActor")
		void StopFire();

	// Połączenie C++ z BP
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ammo")
			void Fire_BP();

	// Połączenie C++ z BP
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ammo")
		void Reload();

	// Strzał pojedyńczego pocisku virtual
	UFUNCTION(BlueprintCallable, Category = "WeaponActor")
		virtual void Fire();
};
