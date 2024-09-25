// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPlayerController.h"

#include "Base.h"
#include "Building.h"
#include "BuildingSpawnLocation.h"
#include "EnhancedInputSubsystems.h"
#include "CustomGameState.h"
#include "Engine/LocalPlayer.h"
#include "CameraPlayer.h"
#include "EnumsList.h"
#include "Kismet/GameplayStatics.h"
#include "Grid.h"
#include "GridPath.h"
#include "GridVisual.h"
#include "Unit.h"

#include "Unit_Child_Leader.h"
#include "Unit_Child_Mage.h"
#include "Unit_Child_Tank.h"
#include "Unit_Child_Warrior.h"



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
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("SelectModeMovement"));
	FindReachableTiles();
	CameraPlayerRef->Path.Add(UnitRef->GetIndexPosition());
	PlayerAction = EDC_ActionPlayer::MoveUnit;
	CameraPlayerRef->IsMovingUnit = true;
}

void ACustomPlayerController::SelectModeAttack()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("SelectModeAttack"));
	PlayerAction = EDC_ActionPlayer::AttackUnit;
	PathReachable = Grid->GridPath->FindTileNeighbors(UnitRef->GetIndexPosition());
	for(FIntPoint Index : PathReachable)
    {
        Grid->GridVisual->addStateToTile(Index, EDC_TileState::Attacked);
    }
}

void ACustomPlayerController::SelectModeSpecial()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("SelectModeSpecial"));
	PlayerAction = EDC_ActionPlayer::SpellCast;
}

void ACustomPlayerController::SelectModeBuilding()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("SelectModeBuilding"));
	PlayerAction = EDC_ActionPlayer::AttackBuilding;
}

void ACustomPlayerController::ControllerInteraction()
{

	if(Grid != nullptr)
	{
		FIntPoint PlayerPositionInGrid = Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PlayerAction : " + FString::FromInt((int)PlayerAction)));
		switch (PlayerAction)
		{
			case EDC_ActionPlayer::None:
				if(Grid->GetGridData()->Find(PlayerPositionInGrid) != nullptr)
				{
					if(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile != nullptr)
					{
						if(!Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile->GetIsSelected()) // Unit
						{
							UnitRef = Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile;
							CameraPlayerRef->SetCustomPlayerController(this);
							IInteractInterface::Execute_Interact(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile, this);
							DisplayWidget();
							GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Turquoise, TEXT("That's not a building"));
						}	
					}
					else if (Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile != nullptr) // Building
					{
						if ((Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile->PlayerOwner == EPlayer::P_Hell && IsHell == true) || (Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile->PlayerOwner == EPlayer::P_Heaven && IsHell == false))
						{
							BuildingRef = Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile;
							for (FIntPoint BuildingIndex : BuildingRef->SpawnLocRef)
							{
								Grid->GridVisual->addStateToTile(BuildingIndex, EDC_TileState::Selected);
							}
							DisplayWidgetBuilding();
							PlayerAction = EDC_ActionPlayer::SelectBuilding;
						}
					}
					else if (Grid->GetGridData()->Find(PlayerPositionInGrid)->BaseOnTile != nullptr) // Base
					{
						if (Grid->GetGridData()->Find(PlayerPositionInGrid)->BaseOnTile->GetIsHell() == IsHell)
						{
							GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Turquoise, TEXT("That's a base"));
							{
								BaseRef = Grid->GetGridData()->Find(PlayerPositionInGrid)->BaseOnTile;
								DisplayWidgetBase();
							}
						}
					}
					else if (Grid->GetGridData()->Find(PlayerPositionInGrid)->TowerOnTile != nullptr)
					{
						TowerRef = Grid->GetGridData()->Find(PlayerPositionInGrid)->TowerOnTile;
						DisplayWidgetTower();
					}

				}
				break;
		case EDC_ActionPlayer::MoveUnit:
				//Move();
					for(FIntPoint Index : PathReachable)
					{
						Grid->GridVisual->RemoveStateFromTile(Index, EDC_TileState::Reachable);
					}
					PathReachable.Empty();
					CameraPlayerRef->IsMovingUnit = false;
					UnitRef->Move(CameraPlayerRef->Path);
					CameraPlayerRef->Path.Empty();
					UnitRef->SetIsSelected(false);
					UnitRef = nullptr;
					PlayerAction = EDC_ActionPlayer::None;
				break;
			case EDC_ActionPlayer::AttackUnit:
				//Attack();
					if(UnitRef)
					{
						if(Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->UnitOnTile != nullptr)
                        {
							if(Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->UnitOnTile->GetPlayerOwner() != UnitRef->GetPlayerOwner())
								UnitRef->AttackUnit(Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->UnitOnTile);
							if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->BaseOnTile)
								UnitRef->AttackBase(Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->BaseOnTile);
                        }
						for(FIntPoint Index : PathReachable)
						{
							Grid->GridVisual->RemoveStateFromTile(Index, EDC_TileState::Attacked);
						}
						PathReachable.Empty();
						UnitRef->SetIsSelected(false);
						UnitRef = nullptr;
						PlayerAction = EDC_ActionPlayer::None;
					}
			break;
			case EDC_ActionPlayer::SpellCast:
				//Spell();
				break;
			case EDC_ActionPlayer::AttackBuilding:
				//AttackBuilding();
				break;
		case EDC_ActionPlayer::SelectBuilding:
				TArray<FIntPoint> AllPossibleSpawns = PrepareSpawnArea(BuildingRef->AllSpawnLoc, BuildingRef->SpawnLocRef[0]);
				BuildingRef->BuildingSpawnLocationRef->DeSpawnGridColors(BuildingRef->AllSpawnLoc);
				for (FIntPoint SpawnIndex : AllPossibleSpawns)
				{
					if (Grid->GetGridData()->Find(PlayerPositionInGrid)->TilePosition == SpawnIndex)
					{
						if (SpawnUnit(BuildingRef->UnitProduced, SpawnIndex))
						{
							PlayerAction = EDC_ActionPlayer::None;
							break;
						}
					}
				}
			PlayerAction = EDC_ActionPlayer::None;
			break;
		}
	}	
}

void ACustomPlayerController::DisplayWidgetBuilding_Implementation()
{
	
}

void ACustomPlayerController::DisplayWidgetBase_Implementation()
{
}

void ACustomPlayerController::DisplayWidgetTower_Implementation()
{
}

TArray<FIntPoint> ACustomPlayerController::PrepareSpawnArea(TArray<FIntPoint> AllSpawnArea, FIntPoint BaseTile)
{
	TArray<FIntPoint> TrueSpawnArea;
	for (FIntPoint Index : AllSpawnArea)
	{
		if (Grid->GridPath->IsValidHeigh(Grid->GetGridData()->Find(Index), Grid->GetGridData()->Find(BaseTile)))
		{
			TrueSpawnArea.Add(Index);
		}
	}
	return TrueSpawnArea;
}

bool ACustomPlayerController::SpawnUnit(EUnitType UnitToSpawn, FIntPoint SpawnChosen)
{
	if (Grid->GetGridData()->Find(SpawnChosen)->UnitOnTile == nullptr)
	{
		AUnit* UnitThatSpawned;
		switch (UnitToSpawn)
		{
		case EUnitType::U_Warrior:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Warrior>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			UnitThatSpawned->Grid = Grid;
			break;
		case EUnitType::U_Mage:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Mage>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			UnitThatSpawned->Grid = Grid;
			break;
		case EUnitType::U_Tank:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Tank>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			UnitThatSpawned->Grid = Grid;
			break;
		case EUnitType::U_Leader:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Leader>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			UnitThatSpawned->Grid = Grid;
			break;
		default:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Warrior>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			UnitThatSpawned->Grid = Grid;
			break;
		}
		return true;
	}
	return false;
		
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
