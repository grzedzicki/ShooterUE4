// Fill out your copyright notice in the Description page of Project Settings.


#include "MP_PlayerStatus.h"

/*
* Liczenie punktów.
*/

void AMP_PlayerStatus::AddScore(float ScoreDelta)
{
	SetScore(GetScore() + ScoreDelta);
}