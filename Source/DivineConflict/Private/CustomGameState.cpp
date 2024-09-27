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
					PlayerState->PlayerTeam = EPlayer::P_Heaven;
				}
				else
				{
					PlayerState->bIsActiveTurn = false;
					PlayerState->OnRep_bIsActiveTurn();
					PlayerState->PlayerTeam = EPlayer::P_Hell;
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
	// Make sure the array has a size of minimum 2
	
	for(APlayerState* CurrentPlayerState : PlayerArray)
	{
		ACustomPlayerState* CurrentCustomPlayerState = Cast<ACustomPlayerState>(CurrentPlayerState);

		if(CurrentCustomPlayerState)
		{
			CurrentCustomPlayerState->bIsActiveTurn = !CurrentCustomPlayerState->bIsActiveTurn;
			CurrentCustomPlayerState->OnRep_bIsActiveTurn();
		}
			
	}
	
	
	/*
	if (AllPlayerStates.Num() < 2)
	{
		AllPlayerStates.SetNum(2, EAllowShrinking::Yes);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, TEXT("PlayerStates set to 2"));
	} 

	// Make sure those PlayerStates exist
	if (AllPlayerStates[bP1Turn] != nullptr)
	{
		//Begin new Turn
		AllPlayerStates[bP1Turn]->NewTurnBegin();
	}
	bP1Turn = !bP1Turn;
	
	if (PlayerControllers.Num() != -1)
	{
		for (ACustomPlayerController* CurrentController : PlayerControllers)
		{
			// Make sure this controller exists
			if (CurrentController != nullptr)
			{
				GEngine->AddOnScreenDebugMessage(-1, 12.f, FColor::Yellow, TEXT("Controller : "+CurrentController->GetPlayerState<ACustomPlayerState>()->GetPlayerName() +"is in turn : "+FString::FromInt(CurrentController->GetIsInActiveTurn())));
				const bool bNewTurnMode = CurrentController->GetIsInActiveTurn();
				CurrentController->SetIsInActiveTurn(!bNewTurnMode);
				if (bNewTurnMode){
					CurrentController->CurrentPA = CurrentController->GetPlayerState<ACustomPlayerState>()->GetMaxActionPoints();
					CurrentController->GetPlayerState<ACustomPlayerState>()->TurnPassed +=1;
				}
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 12.f, FColor::Yellow, TEXT("No controller Skull"));
	}


	if (AllPlayerStates[0] != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("p0 wood = "+FString::FromInt(AllPlayerStates[0]->WoodPoints)));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("p0 stone = "+FString::FromInt(AllPlayerStates[0]->StonePoints)));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("p0 gold = "+FString::FromInt(AllPlayerStates[0]->GoldPoints)));
	}
	if(AllPlayerStates[1] != nullptr){
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("p1 wood = "+FString::FromInt(AllPlayerStates[1]->WoodPoints)));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("p1 stone = "+FString::FromInt(AllPlayerStates[1]->StonePoints)));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("p1 gold = "+FString::FromInt(AllPlayerStates[1]->GoldPoints)));
	}
	OnTurnSwitchDelegate.Broadcast();/*
	for(APlayerState * CurrentPlayerState : PlayerArray)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Player "+CurrentPlayerState->GetPlayerName()));
		ACustomPlayerController* CurrentController = Cast<ACustomPlayerController>(CurrentPlayerState->GetPlayerController());
		
			if(CurrentController->GetIsInActiveTurn())
			{
				CurrentController->SetIsInActiveTurn(false);
			}
			else
			{
				CurrentController->SetIsInActiveTurn(true);
			}
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Player "+CurrentPlayerState->GetPlayerName()+" is in turn :" + FString::SanitizeFloat(CurrentController->GetIsInActiveTurn())));
		
	}*/
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
