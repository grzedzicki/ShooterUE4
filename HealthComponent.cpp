// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "MP_GameMode.h"
#include "Net/UnrealNetwork.h"

/*
*  Ustawanie domyślnych wartości dla tego komponentu
*/
UHealthComponent::UHealthComponent()
{
	// Czy funkcja Tick ma działać
	PrimaryComponentTick.bCanEverTick = true;

	// Domyślna wartość zdrowia
	DefaultHealth = 100.0f;

	bIsDeadMP = false;

	TeamNum = 255;

	// Replikacja
	SetIsReplicatedByDefault(true);
}

/*
* Funkcja wywoływana przy początku gry
* 
* Serwer zarządza zdrowiem graczy, oblicza otrzymane obrażenia itp.
*/
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();

		if (MyOwner) {
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeDamage);
		}
	}
	Health = DefaultHealth;
}

/*
* Funkcja obliczająca zdrowie graczy
*/
void UHealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = Health - OldHealth;

	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

/*
* Funkcja odpowiadająca za otrzymywanie obrażeń
*/
void UHealthComponent::HandleTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (bIsDeadMP || Damage <= 0.0f) return;
	if (DamageCauser != DamagedActor && IsFriendly(DamagedActor, DamageCauser)) return;
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	UE_LOG(LogTemp, Log, TEXT("Health: %s"), *FString::SanitizeFloat(Health));

	//bIsDeadMP = Health <= 0.0f;
	if (Health <= 0.0f) bIsDeadMP = true;
	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

	//DLA WSZYSTKICH GAMEMODE MP
	if (bIsDeadMP)
	{
		AMP_GameMode* GM = Cast<AMP_GameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
	}
}

/*
* Funkcja wywoływana w każdej klatce
*/
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || Health <= 0.0f)
	{
		return;
	}

	Health = FMath::Clamp(Health + HealAmount, 0.0f, DefaultHealth);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(Health), *FString::SanitizeFloat(HealAmount));

	OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);
}

bool UHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if (ActorA == nullptr || ActorB == nullptr)
	{
		// Assume Friendly
		return true;
	}

	UHealthComponent* HealthCompA = Cast<UHealthComponent>(ActorA->GetComponentByClass(UHealthComponent::StaticClass()));
	UHealthComponent* HealthCompB = Cast<UHealthComponent>(ActorB->GetComponentByClass(UHealthComponent::StaticClass()));

	if (HealthCompA == nullptr || HealthCompB == nullptr)
	{
		// Assume friendly
		return true;
	}

	return HealthCompA->TeamNum == HealthCompB->TeamNum;
}

float UHealthComponent::GetHealth() const
{
	return Health;
}

/*
* Replikacja zdrowia u wszystkich graczy
*/
void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, Health);
}