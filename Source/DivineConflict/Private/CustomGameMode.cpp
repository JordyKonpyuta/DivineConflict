// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameMode.h"
#include "CustomGameState.h"
#include "CustomPlayerController.h"
#include "GameFramework/GameSession.h"


void ACustomGameMode::BeginPlay()
{
	Super::BeginPlay();

	GameState = Cast<ACustomGameState>(GetWorld()->GetGameState());
}

void ACustomGameMode::SwitchPlayerTurn()
{
	for (ACustomPlayerController* CurrentController : ThisGameState->PlayerControllers)
	{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Turn Switched"));

			bool bNewTurnMode = CurrentController->GetIsInActiveTurn();
			CurrentController->SetIsInActiveTurn(!bNewTurnMode);
	}
}
