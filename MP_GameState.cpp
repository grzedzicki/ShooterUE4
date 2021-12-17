// Fill out your copyright notice in the Description page of Project Settings.


#include "MP_GameState.h"
#include "Net/UnrealNetwork.h"

void AMP_GameState::SetWaveStatus(EWaveStatus NewState)
{
	if (HasAuthority())
	{
		EWaveStatus OldStatus = WaveStatus;

		WaveStatus = NewState;
		// Call on server
		OnRep_WaveStatus(OldStatus);
	}
}

void AMP_GameState::SetWaveNumber_Implementation(int Number)
{
	WaveNumber = Number;
}
void AMP_GameState::UpdateClientWave_Implementation()
{
}

void AMP_GameState::GameOverState_Implementation()
{
}

void AMP_GameState::OnRep_WaveStatus(EWaveStatus OldState)
{
	WaveStatusChanged(WaveStatus, OldState);
}

void AMP_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMP_GameState, WaveStatus);
}
