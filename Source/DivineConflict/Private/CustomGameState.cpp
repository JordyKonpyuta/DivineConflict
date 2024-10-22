// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameState.h"

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
		true
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
	SwitchPlayerTurn();
}

void ACustomGameState::SwitchIsBlockTurnSwitchTimer()
{
	bBlockTurnSwitch = !bBlockTurnSwitch;
}

void ACustomGameState::SwitchPlayerTurn()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, TEXT("Block Turn Switch : ") + FString::FromInt(bBlockTurnSwitch));
	if(!bBlockTurnSwitch)
	{
		Turn++;
		bBlockTurnSwitch = true;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Switching Turns"));
		for(APlayerState* CurrentPlayerState : PlayerArray)
		{
			ACustomPlayerState* CurrentCustomPlayerState = Cast<ACustomPlayerState>(CurrentPlayerState);
			if(CurrentCustomPlayerState)
			{
				CurrentCustomPlayerState->bIsActiveTurn = !CurrentCustomPlayerState->bIsActiveTurn;
				CurrentCustomPlayerState->SetIsReadyToSwitchTurn(false);
				CurrentCustomPlayerState->OnRep_bIsActiveTurn();
				GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Blue, TEXT("Player Type Turn : ") + FString::FromInt(CurrentCustomPlayerState->bIsActiveTurn));
			}
		}
		// Timer to switch turns                                                           
		GetWorld()->GetTimerManager().ClearTimer(TurnTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(TurnTimerHandle,	this,	&ACustomGameState::BeginTimer,	TurnTimerLength,	true);
		GetWorld()->GetTimerManager().SetTimer(TurnTimerHandle,	this,	&ACustomGameState::SwitchIsBlockTurnSwitchTimer,	BlockTurnSwitchTimerLength,	false);
	}
}

void ACustomGameState::MulticastSwitchPlayerTurn_Implementation()
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
	}
}

void ACustomGameState::CheckPlayerActionPassive()
{
	if(HasAuthority())
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Server"));
	else
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Client"));
		
	for(APlayerState * CurrentPlayerState : PlayerArray)
	{
		if(ACustomPlayerState* PlayerState = Cast<ACustomPlayerState>(CurrentPlayerState))
		{
			if(!PlayerState->bIsActiveTurn)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Passive Turn"));
				//Multi_SendPlayerAction(PlayerState);
				if(ACustomPlayerController* PlayerController = Cast<ACustomPlayerController>(PlayerState->GetPlayerController()))                        
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Passive Turn"));
					PlayerController->Server_ActionPassiveTurn();
				}
			}
		}
	}
}

void ACustomGameState::Multi_SendPlayerAction_Implementation(ACustomPlayerState* PlayerState)
{
	if(ACustomPlayerController* PlayerController = Cast<ACustomPlayerController>(PlayerState->GetPlayerController()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Passive Turn"));
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
					PlayerController->Server_ActionActiveTurn();
				}
			}
		}
	}
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