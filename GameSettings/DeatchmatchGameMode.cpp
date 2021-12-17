// Fill out your copyright notice in the Description page of Project Settings.


#include "DeatchmatchGameMode.h"
#include "HealthComponent.h"
#include "TimerManager.h"
#include "EngineUtils.h"

void ADeatchmatchGameMode::PawnKilled(APawn* PawnKilled)
{
	Super::PawnKilled(PawnKilled);

	UE_LOG(LogTemp, Warning, TEXT("Killed"));

	APlayerController* PlayerController = Cast<APlayerController>(PawnKilled->GetController());

	if (PlayerController != nullptr)
	{
		//PlayerController->GameHasEnded(nullptr, false);
	}
}

void ADeatchmatchGameMode::RestartDeadPlayers_Implementation()
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
