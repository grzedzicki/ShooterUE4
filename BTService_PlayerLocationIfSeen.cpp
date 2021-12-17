﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_PlayerLocationIfSeen.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"

/*
* Serwis wykonywany, gdy gracz znajdzie się w polu widzenia AI.
*/

/*
* Ustalenie nazwy węzła
*/
UBTService_PlayerLocationIfSeen::UBTService_PlayerLocationIfSeen()
{
	NodeName = TEXT("Update Player Location If Seen");
}

/*
* Jeśli AI zauważy gracza ustawia jego pozycję w Blackboard, jeśli gracz zniknie usunie tę wartość.
*/
void UBTService_PlayerLocationIfSeen::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn == nullptr)
	{
		return;
	}

	if (OwnerComp.GetAIOwner() == nullptr)
	{
		return;
	}

	if (OwnerComp.GetAIOwner()->LineOfSightTo(PlayerPawn))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), PlayerPawn->GetActorLocation());

	}
	else
	{
		OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
	}
}
