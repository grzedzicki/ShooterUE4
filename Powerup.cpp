// Fill out your copyright notice in the Description page of Project Settings.


#include "Powerup.h"
#include "Net/UnrealNetwork.h"

/*
* Ustalenie startowych wartości dla Powerupa
* 
*/
APowerup::APowerup()
{
	SetReplicates(true);
	PowerupInterval = 0.0f;
	TotalNrOfTicks = 0;
	bIsPowerupActive = false;
	this->RotationRate = FRotator(0.0f, 180.0f, 0.0f);
}


void APowerup::BeginPlay()
{
	Super::BeginPlay();

}

void APowerup::OnTickPowerup()
{
	TicksProcessed++;

	OnPowerupTicked();

	if (TicksProcessed >= TotalNrOfTicks)
	{
		OnExpired();

		bIsPowerupActive = false;
		OnRep_PowerupActive();

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}
}

/*
* Obracanie obręczy na powierzchnii
*/
void APowerup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	this->AddActorLocalRotation(this->RotationRate * DeltaTime);
}

/*
* Replikacja aktywacji podniesienia powerupa
*/
void APowerup::OnRep_PowerupActive()
{
	OnPowerupStateChanged(bIsPowerupActive);
}

/*
* Aktywacja powerupa, odniesienie dla BP, aby móc przydzielić specjalne efekty dla gracza
* Odliczanie czasu dla odrodzenia nowego powerupa
*/
void APowerup::ActivatePowerup(AActor* ActiveFor)
{
	OnActivated(ActiveFor);
	bIsPowerupActive = true;
	OnRep_PowerupActive();

	if (PowerupInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &APowerup::OnTickPowerup, PowerupInterval, true);
	}
	else
	{
		OnTickPowerup();
	}
}

/*
* Replikacja u wszystkich graczy
*/
void APowerup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APowerup, bIsPowerupActive);
}



