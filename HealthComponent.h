// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UHealthComponent*, HealthComponent, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(MP), meta=(BlueprintSpawnableComponent) )
class SHOOTERUE4_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Ustawanie domyślnych wartości dla tego komponentu
	UHealthComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
		uint8 TeamNum;

protected:
	// Funkcja wywoływana przy początku gry, lub przy odrodzeniu
	virtual void BeginPlay() override;

	bool bIsDeadMP;

	// Replikacja zdrowia u wszystkich klientów
	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "HealthComponent")
		float Health;

	// Domyślna wartość zdrowia
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float DefaultHealth;

	// Zdrowie
	UFUNCTION()
		void OnRep_Health(float OldHealth);

	// Otrzymanie obrażeń
	UFUNCTION()
	void HandleTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:	
	// Funkcja wywoływana w każdej klatce
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Przy zmianie zdrowia
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnHealthChangedSignature OnHealthChanged;

	UFUNCTION(BlueprintCallable, Category = "HealthComponent")
		void Heal(float HealAmount);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Health")
		static bool IsFriendly(AActor* ActorA, AActor* ActorB);

	float GetHealth() const;

};
