// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPlayerController.h"
#include "CameraPlayer.h"


void ACustomPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CameraPlayerRef = Cast<ACameraPlayer>(GetPawn());

	CameraPlayerRef->setCustomePlayerController(this);
	
}

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

bool ACustomPlayerController::GetIsReady()
{
	return IsReady;
}

void ACustomPlayerController::SetIsReady(bool r)
{
	IsReady = r;
}
