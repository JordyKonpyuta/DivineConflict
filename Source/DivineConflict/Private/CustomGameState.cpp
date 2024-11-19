// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameState.h"

#include "CameraPlayer.h"
#include "CustomPlayerController.h"
#include "EnumsList.h"
#include "CustomPlayerState.h"
#include "Net/UnrealNetwork.h"

	// ----------------------------
	// Overrides

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

	// ----------------------------
	// REPLICATIONS !!

void ACustomGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACustomGameState, Turn);
	DOREPLIFETIME(ACustomGameState, PWinner);
	DOREPLIFETIME(ACustomGameState, PLoser);
	DOREPLIFETIME(ACustomGameState, bBlockTurnSwitch);
}

	// ----------------------------
	// Turns

void ACustomGameState::AssignTurnOrder()
{
	AssignPlayerTurns();

	// Timer to switch Turns
	GetWorld()->GetTimerManager().SetTimer(
		TurnTimerHandle,
		this,
		&ACustomGameState::BeginTimer,
		TurnTimerLength,
		false
		);
}

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

void ACustomGameState::BeginTimer()
{
	CheckPlayerActionActive();
	PauseTimerUI();
}

void ACustomGameState::SwitchIsBlockTurnSwitchTimer()
{
	bBlockTurnSwitch = !bBlockTurnSwitch;
}

void ACustomGameState::SwitchPlayerTurn()
{
	if(!bBlockTurnSwitch)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAhAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
		Turn++;
		bBlockTurnSwitch = true;
		for(APlayerState* CurrentPlayerState : PlayerArray)
		{
			ACustomPlayerState* CurrentCustomPlayerState = Cast<ACustomPlayerState>(CurrentPlayerState);
			if(CurrentCustomPlayerState)
			{
				CurrentCustomPlayerState->bIsActiveTurn = !CurrentCustomPlayerState->bIsActiveTurn;
				CurrentCustomPlayerState->bIsBlockTimerNewBeginTurn = false;
				CurrentCustomPlayerState->SetIsReadyToSwitchTurn(false);
				CurrentCustomPlayerState->OnRep_bIsActiveTurn();
			}
		}
		// Timer to switch turns
		GetWorld()->GetTimerManager().ClearTimer(TurnTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(TurnTimerHandle,this,&ACustomGameState::BeginTimer,TurnTimerLength,false);
		GetWorld()->GetTimerManager().SetTimer(SwitchPlayerTurnHandle,this,&ACustomGameState::SwitchIsBlockTurnSwitchTimer,BlockTurnSwitchTimerLength,false);
		Server_SwitchPlayerTurn();
	}
}

void ACustomGameState::Server_SwitchPlayerTurn_Implementation()
{
	OnTurnSwitchDelegate.Broadcast();
}

void ACustomGameState::CheckSwitchPlayerTurn()
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
		//SwitchPlayerTurn();
		CheckPlayerActionActive();
		PauseTimerUI();
	}
}

void ACustomGameState::CheckPlayerActionPassive()
{
	
	for(APlayerState * CurrentPlayerState : PlayerArray)
	{
		if(ACustomPlayerState* PlayerState = Cast<ACustomPlayerState>(CurrentPlayerState))
		{
			if(!PlayerState->bIsActiveTurn)
			{
				//Multi_SendPlayerAction(PlayerState);
				if(ACustomPlayerController* PlayerController = Cast<ACustomPlayerController>(PlayerState->GetPlayerController()))                        
				{
					PlayerController->Server_ActionPassiveTurn();
					return;
				}
			}
		}
	}
	SwitchPlayerTurn();
}

void ACustomGameState::PauseTimerUI()
{
	for(APlayerState * CurrentPlayerState : PlayerArray)
	{
		if(ACustomPlayerState* PlayerState = Cast<ACustomPlayerState>(CurrentPlayerState))
		{
			//Multi_SendPlayerAction(PlayerState);
			if(ACustomPlayerController* PlayerController = Cast<ACustomPlayerController>(PlayerState->GetPlayerController()))                        
			{
				PlayerController->Client_PauseTimerUI();
			}
		}
	}
	GetWorld()->GetTimerManager().ClearTimer(TurnTimerHandle);
}

void ACustomGameState::Multi_SendPlayerAction_Implementation(ACustomPlayerState* PlayerState)
{
	if(ACustomPlayerController* PlayerController = Cast<ACustomPlayerController>(PlayerState->GetPlayerController()))
	{
		PlayerController->Server_ActionPassiveTurn();
	}
}

void ACustomGameState::CheckPlayerActionActive()
{
	for(APlayerState * CurrentPlayerState : PlayerArray)
	{
		if(ACustomPlayerState* PlayerState = Cast<ACustomPlayerState>(CurrentPlayerState))
		{
			if(PlayerState->bIsActiveTurn)
			{
				if(ACustomPlayerController* PlayerController = Cast<ACustomPlayerController>(PlayerState->GetPlayerController()))
				{
					DisableInput(PlayerController);
					PlayerController->Server_ActionActiveTurn();
					PlayerController->Server_ClearMode();
					return;
				}
			}
		}
	}
	CheckPlayerActionPassive();
}

// ----------------------------
	// Widgets

void ACustomGameState::DisplayWidget_Implementation()
{
}

void ACustomGameState::MulticastDisplayWidget_Implementation()
{
	DisplayWidget();
}

void ACustomGameState::ServerDisplayWidget_Implementation()
{
	MulticastDisplayWidget();
}

	// ----------------------------
	// End Game

void ACustomGameState::ServerVictoryScreen_Implementation(EPlayer Loser)
{
	MulticastVictoryScreen(Loser);
	ServerDisplayWidget();
}

void ACustomGameState::MulticastVictoryScreen_Implementation(EPlayer Loser)
{
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (ACustomPlayerState* CustomPlayerState = Cast<ACustomPlayerState>(PlayerState))
        {
            if (CustomPlayerState->PlayerTeam == Loser) PLoser = CustomPlayerState;
            else PWinner = CustomPlayerState;
        }
	}
}