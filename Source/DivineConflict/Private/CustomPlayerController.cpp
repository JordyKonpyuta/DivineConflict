// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPlayerController.h"

bool ACustomPlayerController::GetIsHell()
{
	return IsHell;
}

void ACustomPlayerController::SetIsHell(bool h)
{
	IsHell = h;
}

bool ACustomPlayerController::GetIsInActiveTurn()
{
	return IsInActiveTurn;
}

void ACustomPlayerController::SetIsInActiveTurn(bool a)
{
	IsInActiveTurn = a;
}
