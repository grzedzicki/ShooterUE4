// Fill out your copyright notice in the Description page of Project Settings.


#include "MP_PlayerStatus.h"

/*
* Liczenie punkt�w.
*/

void AMP_PlayerStatus::AddScore(float ScoreDelta)
{
	SetScore(GetScore() + ScoreDelta);
}