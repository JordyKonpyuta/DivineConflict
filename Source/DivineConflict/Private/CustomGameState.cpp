// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameState.h"
#include "CustomPlayerController.h"
#include "CustomPlayerState.h"
#include "CustomGameMode.h"

void ACustomGameState::AssignPlayerTurns()
{
	if (PlayerArray.Num() != -1)
	{
		for(const APlayerState* CurrentPlayerState : PlayerArray)
		{
			if(ACustomPlayerController* CurrentController = Cast<ACustomPlayerController>(CurrentPlayerState->GetPlayerController())){
				if (PlayerArray[0] == CurrentPlayerState)
				{
					
					CurrentController->SetIsInActiveTurn(true);
				}
				else
				{
					CurrentController->SetIsInActiveTurn(false);
				}
			}
		}
	}
}

void ACustomGameState::BeginPlay()
{
	Super::BeginPlay();

	// Timer to give player their turns
	if (HasAuthority()){
		GetWorld()->GetTimerManager().SetTimer(
			BeginningTimerHandle, 
			this,
			&ACustomGameState::AssignTurnOrder,
			LoadingTimer,
			false);
	}
}

void ACustomGameState::SwitchPlayerTurn()
{
	if (PlayerControllers.Num() != -1)
	{
		for (ACustomPlayerController* CurrentController : PlayerControllers)
		{
			const bool bNewTurnMode = CurrentController->GetIsInActiveTurn();
			CurrentController->SetIsInActiveTurn(!bNewTurnMode);
			if (bNewTurnMode){GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("ACTIVE"));}else{GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("NOT ACTIVE"));}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 12.f, FColor::Yellow, TEXT("No controller Skull"));
	}
}

void ACustomGameState::BeginTimer()
{
	SwitchPlayerTurn();
}

void ACustomGameState::AssignTurnOrder()
{
	AssignPlayerTurns();

	// Timer to switch Turns
	GetWorld()->GetTimerManager().SetTimer(
		TurnTimerHandle,
		this,
		&ACustomGameState::BeginTimer,
		TurnTimerLength,
		true);
}
