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
#include "Tower.h"
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
			PathReachable =  Grid->GridPath->NewFindPath(Grid->ConvertLocationToIndex(UnitRef->GetActorLocation()), this);

			for(FIntPoint Index : PathReachable)
			{
				Grid->GridVisual->addStateToTile(Index, EDC_TileState::Reachable);
			}
		}
	
}

void ACustomPlayerController::SelectModeMovement()
{
	FindReachableTiles();
	CameraPlayerRef->Path.Add(UnitRef->GetIndexPosition());
	PlayerAction = EDC_ActionPlayer::MoveUnit;
	CameraPlayerRef->IsMovingUnit = true;
}

void ACustomPlayerController::SelectModeAttack()
{
	PlayerAction = EDC_ActionPlayer::AttackUnit;
	PathReachable = Grid->GridPath->FindTileNeighbors(UnitRef->GetIndexPosition());
	for(FIntPoint Index : PathReachable)
    {
        Grid->GridVisual->addStateToTile(Index, EDC_TileState::Attacked);
    }
}

void ACustomPlayerController::SelectModeSpecial()
{
	switch (UnitRef->UnitName)
	{
	case EUnitName::Warrior:
		//Warrior
			UnitRef->Special();
		break;
	case EUnitName::Tank:
		//Tank
			UnitRef->Special();
		break;
	case EUnitName::Mage:
		//Mage
			PathReachable =  Grid->GridPath->FindPath(UnitRef->GetIndexPosition(),FIntPoint(-999,-999),true,3,false);
			for(FIntPoint Index : PathReachable)
			{
				Grid->GridVisual->addStateToTile(Index, EDC_TileState::Attacked);
			}
			PlayerAction = EDC_ActionPlayer::SpellCast;
		break;
	default: ;
	}
	
	
	
}

void ACustomPlayerController::SelectModeBuilding()
{
	PlayerAction = EDC_ActionPlayer::AttackBuilding;
}

void ACustomPlayerController::ControllerInteraction()
{
	if(!PlayerStateRef)
		PlayerStateRef = Cast<ACustomPlayerState>(PlayerState);
	
	if(Grid != nullptr)
	{
		FIntPoint PlayerPositionInGrid = Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation());
		if(CurrentPA >= 1)
		{
			if (PlayerAction != EDC_ActionPlayer::None) CurrentPA -= 1;
			switch (PlayerAction)
			{
			case EDC_ActionPlayer::None:
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("None"));
				if(Grid->GetGridData()->Find(PlayerPositionInGrid) != nullptr)
				{
					if (Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile != nullptr && !PlayerStateRef->bIsActiveTurn) // Building, Passive Turn
					{
						if ((Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile->PlayerOwner == EPlayer::P_Hell && IsHell == true)
							|| (Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile->PlayerOwner == EPlayer::P_Heaven && IsHell == false)
							)
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
					else if (Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile != nullptr && PlayerStateRef->bIsActiveTurn) // Building, Active Turn
					{
						if (((Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile->PlayerOwner == EPlayer::P_Hell && PlayerTeam == EPlayer::P_Hell)
							|| (Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile->PlayerOwner == EPlayer::P_Heaven && PlayerTeam == EPlayer::P_Heaven))
							&& Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile->GarrisonFull)
						{
							UnitRef = Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile->UnitRef;
							CameraPlayerRef->SetCustomPlayerController(this);
							IInteractInterface::Execute_Interact(Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile->UnitRef, this);
							DisplayWidget();
						}
					}
					else if(Grid->GridData.Find(PlayerPositionInGrid)->UnitOnTile != nullptr)
					{
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Unit"));
						if(!Grid->GridData.Find(PlayerPositionInGrid)->UnitOnTile->GetIsSelected() && PlayerStateRef->bIsActiveTurn/* && Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile->GetPlayerOwner() == PlayerTeam*/) // Unit
						{
							UE_LOG( LogTemp, Warning, TEXT("Unit Selected") );
							UnitRef = Grid->GridData.Find(PlayerPositionInGrid)->UnitOnTile;
							UE_LOG( LogTemp, Warning, TEXT("Unit Selected") );
							CameraPlayerRef->SetCustomPlayerController(this);
							UE_LOG( LogTemp, Warning, TEXT("Unit Selected") );
							IInteractInterface::Execute_Interact(Grid->GridData.Find(PlayerPositionInGrid)->UnitOnTile, this);
							UE_LOG( LogTemp, Warning, TEXT("Unit Selected") );
							DisplayWidget();
							UE_LOG( LogTemp, Warning, TEXT("Unit Selected") );
						}
					}
					else if (Grid->GetGridData()->Find(PlayerPositionInGrid)->BaseOnTile != nullptr && !PlayerStateRef->bIsActiveTurn) // Base
					{
						if (Grid->GetGridData()->Find(PlayerPositionInGrid)->BaseOnTile->GetIsHell() == IsHell)
						{
							{
								BaseRef = Grid->GetGridData()->Find(PlayerPositionInGrid)->BaseOnTile;
								BaseRef->IsSelected = true;
								BaseRef->VisualSpawn();
								DisplayWidgetBase();
							}
						}
					}
					else if (Grid->GetGridData()->Find(PlayerPositionInGrid)->TowerOnTile != nullptr && PlayerStateRef->bIsActiveTurn) // Tower
					{
						TowerRef = Grid->GetGridData()->Find(PlayerPositionInGrid)->TowerOnTile;
						TowerRef->IsSelected = true;
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
					UnitRef->SetIsSelected(false);
					ServerMoveUnit(CameraPlayerRef->Path,UnitRef);
					PathReachable.Empty();
					CameraPlayerRef->IsMovingUnit = false;
					CameraPlayerRef->Path.Empty();
					
					
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
						if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->BuildingOnTile != nullptr)
						{
							if(Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->BuildingOnTile->UnitRef->GetPlayerOwner() != UnitRef->GetPlayerOwner())
							{
								UnitRef->AttackUnit(Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->BuildingOnTile->UnitRef);
							}
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
					if(UnitRef)
					{
						if(Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->UnitOnTile != nullptr)
						{
							if(Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->UnitOnTile->GetPlayerOwner() != UnitRef->GetPlayerOwner())
								UnitRef->SpecialUnit(Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->UnitOnTile);
							if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->BaseOnTile)
								UnitRef->SpecialBase(Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->BaseOnTile);
						}
						for(FIntPoint Index : PathReachable)
						{
							Grid->GridVisual->RemoveStateFromTile(Index, EDC_TileState::Attacked);
						}
						PathReachable.Empty();
						UnitRef->SetIsSelected(false);
						UnitRef = nullptr;
						PlayerAction = EDC_ActionPlayer::None;
						break;
						case EDC_ActionPlayer::AttackBuilding:
							//AttackBuilding();
								if(TowerRef)
								{
									if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->UnitOnTile != nullptr)
									{
										if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->UnitOnTile->GetPlayerOwner() == EPlayer::P_Heaven && TowerRef->GetIsHell() || Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->UnitOnTile->GetPlayerOwner() == EPlayer::P_Hell && !TowerRef->GetIsHell())
											TowerRef->AttackUnit(Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->UnitOnTile);
									}
									PathReachable = TowerRef->TilesInRange;
									TowerRef->TilesInRange.Empty();
									for (FIntPoint Index : PathReachable)
									{
										Grid->GridVisual->RemoveStateFromTile(Index, EDC_TileState::Attacked);
									}
									PathReachable.Empty();
									TowerRef = nullptr;
									PlayerAction = EDC_ActionPlayer::None;
								}
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
									GetPlayerState<ACustomPlayerState>()->SetUnits(GetPlayerState<ACustomPlayerState>()->GetUnits() + 1);
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
	}
}

TArray<FIntPoint> ACustomPlayerController::GetPathReachable()
{
	return PathReachable;
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
		AUnit* UnitThatSpawned = nullptr;
		switch (UnitToSpawn)
		{
		case EUnitType::U_Warrior:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Warrior>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			UnitThatSpawned->Grid = Grid;
			if (IsHell)
			{
				UnitThatSpawned->SetPlayerOwner(EPlayer::P_Hell);
			}
			else
			{
				UnitThatSpawned->SetPlayerOwner(EPlayer::P_Heaven);
			}
			break;
		case EUnitType::U_Mage:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Mage>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			UnitThatSpawned->Grid = Grid;
			if (IsHell)
			{
				UnitThatSpawned->SetPlayerOwner(EPlayer::P_Hell);
			}
			else
			{
				UnitThatSpawned->SetPlayerOwner(EPlayer::P_Heaven);
			}
			break;
		case EUnitType::U_Tank:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Tank>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			UnitThatSpawned->Grid = Grid;
			if (IsHell)
			{
				UnitThatSpawned->SetPlayerOwner(EPlayer::P_Hell);
			}
			else
			{
				UnitThatSpawned->SetPlayerOwner(EPlayer::P_Heaven);
			}
			break;
		case EUnitType::U_Leader:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Leader>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			UnitThatSpawned->Grid = Grid;
			if (IsHell)
			{
				UnitThatSpawned->SetPlayerOwner(EPlayer::P_Hell);
			}
			else
			{
				UnitThatSpawned->SetPlayerOwner(EPlayer::P_Heaven);
			}
			break;
		default:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Warrior>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			UnitThatSpawned->Grid = Grid;
			if (IsHell)
			{
				UnitThatSpawned->SetPlayerOwner(EPlayer::P_Hell);
			}
			else
			{
				UnitThatSpawned->SetPlayerOwner(EPlayer::P_Heaven);
			}
			break;
		}
		return true;

		
	}
	return false;
		
}

void ACustomPlayerController::EndTurn()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("EndTurn"));
	Server_EndTurn();
}

void ACustomPlayerController::Server_EndTurn_Implementation()
{
	Cast<ACustomGameState>(GetWorld()->GetGameState())->SwitchPlayerTurn();
}

void ACustomPlayerController::ServerMoveUnit_Implementation(const TArray<FIntPoint> &PathToMove, const AUnit* UnitRefServer)
{
	AUnit* RefUnit = const_cast<AUnit*>(UnitRefServer);
	
	if(UnitRefServer)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("ServerMoveUnit"));
		RefUnit->Move(PathToMove);
		UnitRef = nullptr;
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

EPlayer ACustomPlayerController::GetPlayerTeam()
{
	return PlayerTeam;
}

void ACustomPlayerController::SetPlayerTeam(EPlayer PT)
{
	PlayerTeam = PT;
}

bool ACustomPlayerController::GetSelectingUnitFromBuilding()
{
	return IsSelectingUnitFromBuilding;
}

void ACustomPlayerController::SetSelectingUnitFromBuilding(bool bS)
{
	IsSelectingUnitFromBuilding = bS;
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

void ACustomPlayerController::SetPlayerAction(EDC_ActionPlayer PA)
{
	PlayerAction = PA;
}
