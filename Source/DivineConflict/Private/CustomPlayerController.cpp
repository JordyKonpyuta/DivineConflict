// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "CustomGameState.h"
#include "Engine/LocalPlayer.h"
#include "CameraPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Grid.h"
#include "GridPath.h"
#include "GridVisual.h"
#include "Unit.h"



void ACustomPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	CameraPlayerRef = Cast<ACameraPlayer>(GetPawn());
	
	if(CameraPlayerRef != nullptr)
	{
		CameraPlayerRef->SetCustomPlayerController(this);
	}
	
	// Send Controller to Instance
	if(ACustomGameState* GameState = Cast<ACustomGameState>(GetWorld()->GetGameState()))
	{
		GameState->PlayerControllers.Add(this);
	}

	setGrid();
}

void ACustomPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (APlayerController* PlayerController = Cast<APlayerController>(this))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
}

void ACustomPlayerController::setGrid()
{
	//get the grid
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Set Grid"));
	AActor* FoundActors = UGameplayStatics::GetActorOfClass(GetWorld(), AGrid::StaticClass());
	if (FoundActors != nullptr)
	{
		Grid = Cast<AGrid>(FoundActors);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Grid Found"));
		//delay 0.2s and try again
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACustomPlayerController::setGrid, 0.2f, false);
		
	}
}

void ACustomPlayerController::FindReachableTiles()
{
	if(Grid)
		
		if(UnitRef)
		{
			PathReachable =  Grid->GridPath->FindPath(Grid->ConvertLocationToIndex(UnitRef->GetActorLocation()), FIntPoint(-999,-999), true,UnitRef->GetPM(),false);

			for(FIntPoint Index : PathReachable)
			{
				Grid->GridVisual->addStateToTile(Index, EDC_TileState::Reachable);
			}
		}
	
}

void ACustomPlayerController::SelectModeMovement()
{
	FindReachableTiles();
	PlayerAction = EDC_ActionPlayer::MoveUnit;
	CameraPlayerRef->IsMovingUnit = true;
}

void ACustomPlayerController::SelectModeAttack()
{
	PlayerAction = EDC_ActionPlayer::AttackUnit;
}

void ACustomPlayerController::SelectModeSpecial()
{
	PlayerAction = EDC_ActionPlayer::SpellCast;
}

void ACustomPlayerController::SelectModeBuilding()
{
	PlayerAction = EDC_ActionPlayer::AttackBuilding;
}

void ACustomPlayerController::ControllerInteraction()
{

	if(Grid != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PlayerAction : " + FString::FromInt((int)PlayerAction)));
		switch (PlayerAction)
		{
			case EDC_ActionPlayer::None:

				FIntPoint PlayerPositionInGrid = Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation());
		
				if(Grid->GetGridData()->Find(PlayerPositionInGrid) != nullptr)
				{
					if(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile != nullptr)
					{
						if(!Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile->GetIsSelected())
						{
							UnitRef = Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile;
							CameraPlayerRef->SetCustomPlayerController(this);
							IInteractInterface::Execute_Interact(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile, this);
							DisplayWidget();
						}	
					}
				}
				break;
		case EDC_ActionPlayer::MoveUnit:
				//Move();
					for(FIntPoint Index : PathReachable)
					{
						Grid->GridVisual->RemoveStateFromTile(Index, EDC_TileState::Reachable);
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Remove State Tile : " + Index.ToString()));
					}
					PathReachable.Empty();
					CameraPlayerRef->IsMovingUnit = false;
					UnitRef->Move();
					UnitRef->SetIsSelected(false);
					UnitRef = nullptr;
					PlayerAction = EDC_ActionPlayer::None;
				break;
			case EDC_ActionPlayer::AttackUnit:
				//Attack();
			break;
			case EDC_ActionPlayer::SpellCast:
				//Spell();
				break;
			case EDC_ActionPlayer::AttackBuilding:
				//AttackBuilding();
				break;
		}
	}	
}

/*void ACustomPlayerController::DisplayWidgetEndGame_Implementation()
{
}*/

void ACustomPlayerController::DisplayWidget_Implementation()
{
}

bool ACustomPlayerController::GetIsHell()
{
	return IsHell;
}

void ACustomPlayerController::SetIsHell(bool bH)
{
	IsHell = bH;
}

bool ACustomPlayerController::GetIsInActiveTurn()
{
	return IsInActiveTurn;
}

void ACustomPlayerController::SetIsInActiveTurn(bool bA)
{
	IsInActiveTurn = bA;
}

bool ACustomPlayerController::GetIsReady()
{
	return IsReady;
}

void ACustomPlayerController::SetIsReady(bool bR)
{
	IsReady = bR;
}

bool ACustomPlayerController::GetIsDead()
{
	return IsDead;
}

void ACustomPlayerController::SetIsDead(bool bD)
{
	IsDead = bD;
}
