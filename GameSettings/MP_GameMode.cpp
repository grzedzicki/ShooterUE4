// Fill out your copyright notice in the Description page of Project Settings.


#include "MP_GameMode.h"
#include "HealthComponent.h"
#include "MP_GameState.h"
#include "MP_PlayerStatus.h"
#include "TimerManager.h"
#include "EngineUtils.h"

/*
  Domyślne ustawienia dla Tower Defense
*/
AMP_GameMode::AMP_GameMode()
{
	TimeBetweenWaves = 2.0f;

	GameStateClass = AMP_GameState::StaticClass();
	PlayerStateClass = AMP_PlayerStatus::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
}

void AMP_GameMode::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();
}

void AMP_GameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckWaveState();
	CheckAnyPlayerAlive();
}

/*
  Spawn przeciwników, odliczenie ich ilości
*/
void AMP_GameMode::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	NrOfBotsToSpawn--;

	if (NrOfBotsToSpawn <= 0)
	{
		EndWave();
	}
}

/*
  Poinformowanie odpowiadającego Blueprinta o początku nowej fali
  Inkrementacja fali, ustalenie ilości przeciwników do odrodzenia, start timera dla przeciwników
*/
void AMP_GameMode::StartWave()
{
	StartWaveForBP();
	AMP_GameState* GS = GetGameState<AMP_GameState>();
	if (ensureAlways(GS))
	{
		GS->SetWaveNumber(WaveCount);
	}
	WaveCount++;

	NrOfBotsToSpawn = 2 * WaveCount;

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &AMP_GameMode::SpawnBotTimerElapsed, 1.0f, true, 0.0f);

	SetWaveStatus(EWaveStatus::WaveInProgress);
}

/*
  Czyszczenie timera danej fali, ustalenie statusu fali
*/
void AMP_GameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);

	SetWaveStatus(EWaveStatus::WaitingToComplete);
}

/*
  Start timera dla nowej fali, odrodzenie zmarłych graczy
*/
void AMP_GameMode::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &AMP_GameMode::StartWave, TimeBetweenWaves, false);

	SetWaveStatus(EWaveStatus::PrepareNextWave);

	RestartDeadPlayers();
}

/*
  Funkcja wywoływana cyklicznie sprawdzająca czy wszyscy przeciwnicy zostali pokonani
*/
void AMP_GameMode::CheckWaveState()
{
	bool bIsPreparingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

	if (NrOfBotsToSpawn > 0 || bIsPreparingForWave)
	{
		return;
	}

	bool bIsAnyBotAlive = false;

	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		APawn* TestPawn = *It;
		if (TestPawn == nullptr || TestPawn->IsPlayerControlled())
		{
			continue;
		}

		UHealthComponent* HealthComponent = Cast<UHealthComponent>(TestPawn->GetComponentByClass(UHealthComponent::StaticClass()));
		if (HealthComponent && HealthComponent->GetHealth() > 0.0f)
		{
			bIsAnyBotAlive = true;
			break;
		}
	}

	if (!bIsAnyBotAlive)
	{
		SetWaveStatus(EWaveStatus::WaveComplete);

		PrepareForNextWave();
	}
}

/*
  Ustalenie statusu fali
*/
void AMP_GameMode::SetWaveStatus(EWaveStatus NewStatus)
{
	AMP_GameState* GS = GetGameState<AMP_GameState>();
	if (ensureAlways(GS))
	{
		GS->SetWaveStatus(NewStatus);
	}
}

/*
  Funkcja sprawdzająca czy są jeszcze żyjący gracze, jeśli nie, koniec gry
*/
void AMP_GameMode::CheckAnyPlayerAlive()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn())
		{
			APawn* MyPawn = PC->GetPawn();
			UHealthComponent* HealthComponent = Cast<UHealthComponent>(MyPawn->GetComponentByClass(UHealthComponent::StaticClass()));
			if (ensure(HealthComponent) && HealthComponent->GetHealth() > 0.0f)
			{
				return;
			}
		}
	}

	GameOver();
}

/*
  W przypadku końca gry, wysłanie do Blueprint o tym informacji
*/
void AMP_GameMode::GameOver()
{
	EndWave();
	SetWaveStatus(EWaveStatus::GameOver);

	GameOverForBP();
	AMP_GameState* GS = GetGameState<AMP_GameState>();
	if (ensureAlways(GS))
	{
		GS->GameOverState();
	}
}

/*
  Funkcje dla Blueprint
*/
void AMP_GameMode::GameOverForBP_Implementation()
{
}

void AMP_GameMode::StartWaveForBP_Implementation()
{
}

void AMP_GameMode::RestartDeadPlayers_Implementation()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn() == nullptr)
		{
			RestartPlayer(PC);
		}
	}
}


