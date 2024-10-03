// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameState.h"
#include "EnumsList.h"
#include "CustomPlayerController.h"
#include "CustomPlayerState.h"

void ACustomGameState::AssignPlayerTurns()
{

	if (PlayerArray.Num() != -1)
	{
		for(APlayerState* CurrentPlayerState : PlayerArray)
		{
			if(ACustomPlayerState* PlayerState = Cast<ACustomPlayerState>(CurrentPlayerState)){
				if (PlayerArray[0] == CurrentPlayerState)
				{
					
					PlayerState->bIsActiveTurn = true;
					PlayerState->OnRep_bIsActiveTurn();
					PlayerState->PlayerTeam = EPlayer::P_Hell;
				}
				else
				{
					PlayerState->bIsActiveTurn = false;
					PlayerState->OnRep_bIsActiveTurn();
					PlayerState->PlayerTeam = EPlayer::P_Heaven;
				}
			}
		}
	}
	// Problème : le Broadcast ne s'effectue que sur le serveur. 
	OnTurnSwitchDelegate.Broadcast();
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
	int PlayerReadyCount = 0;
	// Make sure the array has a size of minimum 2
	for(APlayerState* CurrentPlayerState : PlayerArray)
	{
		if(ACustomPlayerState* PlayerState = Cast<ACustomPlayerState>(CurrentPlayerState)){
			if (PlayerState->GetIsReadyToSwitchTurn())
	           	PlayerReadyCount++;
		}
	}
	if(PlayerReadyCount == PlayerArray.Num())
    {
		for(APlayerState* CurrentPlayerState : PlayerArray)
		{
			ACustomPlayerState* CurrentCustomPlayerState = Cast<ACustomPlayerState>(CurrentPlayerState);
			if(CurrentCustomPlayerState)
			{
				CurrentCustomPlayerState->bIsActiveTurn = !CurrentCustomPlayerState->bIsActiveTurn;
				CurrentCustomPlayerState->SetIsReadyToSwitchTurn(false);
				CurrentCustomPlayerState->OnRep_bIsActiveTurn();
			}
		}
    }
	
	//OnTurnSwitchDelegate.Broadcast();
	MulticastSwitchPlayerTurn();
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

void ACustomGameState::MulticastSwitchPlayerTurn_Implementation()
{
	OnTurnSwitchDelegate.Broadcast();
}
