// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"


class UCameraComponent;
class USpringArmComponent;
class AWeaponActor;
class UHealthComponent;

UCLASS()
class SHOOTERUE4_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Ustawanie domyślnych wartości dla tej klasy
	AMainCharacter();

protected:
	// Funkcja wywoływana przy początku gry, lub przy odrodzeniu
	virtual void BeginPlay() override;

	//Poruszanie się
	void MoveForward(float Value);
	void MoveRight(float Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character Movement")
	void BeginCrouch();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement")
	void EndCrouch();

	//Przybliżenie kamery
	void BeginZoom();
	void EndZoom();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Zoom")
	bool bZoom;

	// Dostęp do klasy Weapon
	UPROPERTY(Replicated, BlueprintReadWrite)
	AWeaponActor* Weapon;

	//Przydzielenie broni graczowi
	UPROPERTY(EditAnywhere, Category = "Player")
	TSubclassOf<AWeaponActor> StarterWeapon;

	//Łączenie mesha broni z postacią
	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName;

	void Fire();

	//Wartość przybliżenia kamery
	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomFOV;

	//
	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClamMax = 100))
		float ZoomInterpSpeed;

	//Domyślna wartość kamery ( bez przybliżenia)
	float DefaultFOV;

	//Komponent kamery - gracz
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UCameraComponent* CameraComponent;

	//Komponent SpringArm, który jest ramieniem, na którym opiera się kamera (widok z 3 osoby)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USpringArmComponent* SpringArmComponent;

	//Komponent zdrowia
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UHealthComponent* HealthComponent;


public:	
	// Funkcja wywoływana w każdej klatce
	virtual void Tick(float DeltaTime) override;

	// Bindowanie poruszania się gracza
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Ammo")
		void Reload();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ammo")
		void CharacterFire();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ammo")
		void ClearWeapons();

	UFUNCTION(BlueprintCallable, Category = "Player")
		void ShootAI();

	// Funkcja ognia automatycznego
	UFUNCTION(BlueprintCallable, Category = "Player")
		void StartFire();

	// Przerwanie ognia automatycznego
	UFUNCTION(BlueprintCallable, Category = "Player")
		void StopFire();

	// Otrzymywanie obrażenie
	UFUNCTION()
		void OnHealthChanged(UHealthComponent* OHealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	// Replikowanie śmierci danego gracza u innych klientów
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bDied;

	//
	virtual FVector GetPawnViewLocation() const override;
};
