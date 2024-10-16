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
#include "Net/UnrealNetwork.h"
#include "Utility/DC_CustomPlayerStart.h"


void ACustomPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UpdateWidget3D(false, false);
	
	CameraPlayerRef = Cast<ACameraPlayer>(GetPawn());
	
	if(CameraPlayerRef != nullptr)
	{
		CameraPlayerRef->SetCustomPlayerController(this);
	}
	setGrid();
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACustomPlayerController::AssignPlayerPosition, 2.f, false);
}

void ACustomPlayerController::GetLifetimeReplicatedProps( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACustomPlayerController, PlayerTeam);
	DOREPLIFETIME(ACustomPlayerController, BaseRef);
	DOREPLIFETIME(ACustomPlayerController, BuildingRef);

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

void ACustomPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACustomPlayerController::OnRep_PlayerTeam()
{
	AssignPlayerPosition();
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
	CameraPlayerRef->IsAttacking = true;
	


	if (UnitRef->HasMoved)
	{
		PathReachable = Grid->GridPath->FindTileNeighbors(
			Grid->ConvertLocationToIndex(UnitRef->GetFinalGhostMesh()->GetComponentLocation()));
		CameraPlayerRef->FullMoveDirection.X = UnitRef->GetFinalGhostMesh()->GetComponentLocation().X;
		CameraPlayerRef->FullMoveDirection.Y = UnitRef->GetFinalGhostMesh()->GetComponentLocation().Y;
		CameraPlayerRef->FullMoveDirection.Z = (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 175;

	} else
	{
		PathReachable = Grid->GridPath->FindTileNeighbors(UnitRef->GetIndexPosition());
	}
	
	
	for(FIntPoint Index : PathReachable)
    {
        Grid->GridVisual->addStateToTile(Index, EDC_TileState::Attacked);
    }
	
	PathReachable.Add(UnitRef->GetIndexPosition());
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

void ACustomPlayerController::SelectModeAttackBuilding()
{
	PlayerAction = EDC_ActionPlayer::AttackBuilding;
	PathReachable = Grid->GridPath->FindPath(TowerRef->GetGridPosition(),FIntPoint(-999,-999),true,3,false);
	for(FIntPoint Index : PathReachable)
	{
		Grid->GridVisual->addStateToTile(Index, EDC_TileState::Attacked);
	}
}


void ACustomPlayerController::ControllerInteraction()
{
	if(!PlayerStateRef)
	{
		PlayerStateRef = Cast<ACustomPlayerState>(PlayerState);
	}

	if (Grid != nullptr)
	{
		TArray<FIntPoint> AllPossibleSpawns;
		FIntPoint PlayerPositionInGrid = Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation());
		switch (PlayerAction)
		{
		case EDC_ActionPlayer::None:
			if(Grid->GetGridData()->Find(PlayerPositionInGrid) != nullptr) 
			{
				
				if(PlayerStateRef->bIsActiveTurn)
				{
					//Active Turn
					if(Grid->GetGridData()->Find(PlayerPositionInGrid)->TowerOnTile)
					{
						if(Grid->GetGridData()->Find(PlayerPositionInGrid)->TowerOnTile->GetPlayerOwner() == PlayerStateRef->PlayerTeam)
						{
							TowerRef = Grid->GetGridData()->Find(PlayerPositionInGrid)->TowerOnTile;
							TowerRef->IsSelected = true;
							TowerRef->PlayerController = this;
							DisplayWidgetTower();
						}
					}
						

					else if (Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile)
					{
						if(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile->GetPlayerOwner() == PlayerStateRef->PlayerTeam)
						{
							UnitRef = Grid->GridData.Find(PlayerPositionInGrid)->UnitOnTile;
							CameraPlayerRef->SetCustomPlayerController(this);
							CameraPlayerRef->UnitMovingCurrentMovNumber = UnitRef->GetPM();
							IInteractInterface::Execute_Interact(Grid->GridData.Find(PlayerPositionInGrid)->UnitOnTile, this);
							DisplayWidget();
						}
					}
					else if(Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile)
					{
						if(Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile->UnitRef && Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile->PlayerOwner == PlayerStateRef->PlayerTeam)
						{
                                UnitRef = Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile->UnitRef;
                                CameraPlayerRef->SetCustomPlayerController(this);
                                CameraPlayerRef->UnitMovingCurrentMovNumber = UnitRef->GetPM();
                                IInteractInterface::Execute_Interact(Grid->GridData.Find(PlayerPositionInGrid)->BuildingOnTile->UnitRef, this);
                                DisplayWidget();
						}
					}
				}
				else
				{
					//PassiveTurn
					GEngine->AddOnScreenDebugMessage(-1, 5.f,FColor::Blue,TEXT("PassiveTurn"));
					if(Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile)
					{
						if(Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile->PlayerOwner == PlayerStateRef->PlayerTeam)
						{
							BuildingRef = Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile;
							AllCurrentSpawnPoints = BuildingRef->AllSpawnLoc;
							AllCurrentSpawnPoints += BuildingRef->SpawnLocRef;
							for (FIntPoint BuildingIndex : BuildingRef->SpawnLocRef)
							{
								Grid->GridVisual->addStateToTile(BuildingIndex, EDC_TileState::Selected);
							}
							DisplayWidgetBuilding();
						}
					}	
					else if(Grid->GetGridData()->Find(PlayerPositionInGrid)->BaseOnTile)
					{
						GEngine->AddOnScreenDebugMessage(-1, 5.f,FColor::Blue,TEXT("Base"));
						if(Grid->GetGridData()->Find(PlayerPositionInGrid)->BaseOnTile->PlayerOwner == PlayerStateRef->PlayerTeam)
						{
							BaseRef = Grid->GetGridData()->Find(PlayerPositionInGrid)->BaseOnTile;
							BaseRef->IsSelected = true;
							BaseRef->VisualSpawn();
							DisplayWidgetBase();
							GEngine->AddOnScreenDebugMessage(-1, 5.f,FColor::Blue,TEXT("BaseRef : " + BaseRef->GetName()));
						}
					}
					
				}
			}
			break;
		case EDC_ActionPlayer::AttackBuilding:
			if(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile)
			{
				if(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile->GetPlayerOwner() != PlayerStateRef->PlayerTeam)
				{
					TowerRef->PreprareAttack(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile);
					AllPlayerActions.Add(FStructActions(TowerRef, EDC_ActionPlayer::AttackBuilding, Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile));
					PlayerAction = EDC_ActionPlayer::None;
				}
			}
			break;
		case EDC_ActionPlayer::AttackUnit:
			if (UnitRef)
			{
				if (!UnitRef->HasActed){
					// Are we attacking a Unit?
					if(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile)
					{
						if (Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile->GetPlayerOwner() != PlayerStateRef->PlayerTeam)
						{
							UnitRef->PrepareAttackUnit(PlayerPositionInGrid);
							AllPlayerActions.Add(FStructActions(UnitRef, EDC_ActionPlayer::AttackUnit, Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile));
						}
					}
					// Are we attacking a Building?
					else if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->BuildingOnTile)
					{
						if(Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->BuildingOnTile->PlayerOwner != PlayerStateRef->PlayerTeam)
						{
							GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("AttackBuilding"));
							if(Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->BuildingOnTile->GarrisonFull)
							{
								GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("AttackBuilding"));
								UnitRef->PrepareAttackUnit(PlayerPositionInGrid);
								AllPlayerActions.Add(FStructActions(UnitRef, EDC_ActionPlayer::AttackUnit, Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile->UnitRef));
							}
						}
					}
					// Are we attacking a Base?
					else if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->BaseOnTile)
					{
						if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->BaseOnTile->PlayerOwner != UnitRef->GetPlayerOwner())
						{
							//AttackBase(Grid->GetGridData()->Find(PlayerPositionInGrid)->BaseOnTile, UnitRef);
							AllPlayerActions.Add(FStructActions(Grid->GetGridData()->Find(PlayerPositionInGrid)->BaseOnTile, EDC_ActionPlayer::AttackBuilding,UnitRef));
						}
					}
					for(FIntPoint Index : PathReachable)
					{
						Grid->GridVisual->RemoveStateFromTile(Index, EDC_TileState::Attacked);
					}
					PathReachable.Empty();
					UnitRef->SetIsSelected(false);
					UnitRef->HasActed = true;
					UnitRef = nullptr;
					CameraPlayerRef->IsAttacking = false;
					PlayerAction = EDC_ActionPlayer::None;
				}
			}
			break;
		case EDC_ActionPlayer::MoveUnit:
			if (UnitRef)
			{
				if (!UnitRef->HasMoved)
				{
					for(FIntPoint Index : PathReachable)
					{
						Grid->GridVisual->RemoveStateFromTile(Index, EDC_TileState::Reachable);
					}
					for(FIntPoint Index : CameraPlayerRef->Path)
					{
						Grid->GridVisual->RemoveStateFromTile(Index, EDC_TileState::Pathfinding);
					}

					if (CameraPlayerRef->Path.Num() > 1)
					{
						UnitRef->SetIsSelected(false);
						Server_PrepareMoveUnit(CameraPlayerRef->Path,UnitRef);
						AllPlayerActions.Add(FStructActions(UnitRef, EDC_ActionPlayer::MoveUnit));
						UnitRef->HasMoved = true;
					}
					PathReachable.Empty();
					CameraPlayerRef->IsMovingUnit = false;
					CameraPlayerRef->Path.Empty();
					UpdateUi();
					
					PlayerAction = EDC_ActionPlayer::None;
				}
			}
			break;
		case EDC_ActionPlayer::SelectBuilding:
			AllPossibleSpawns = PrepareSpawnArea(BuildingRef->AllSpawnLoc, BuildingRef->SpawnLocRef[0]);
			BuildingRef->BuildingSpawnLocationRef->DeSpawnGridColors(BuildingRef->AllSpawnLoc);
			AllCurrentSpawnPoints.Empty();
			CameraPlayerRef->IsSpawningUnit = false;
			for (FIntPoint SpawnIndex : AllPossibleSpawns)
			{
				if (Grid->GetGridData()->Find(PlayerPositionInGrid)->TilePosition == SpawnIndex)
				{
					if (SpawnUnit(BuildingRef->UnitProduced, SpawnIndex,nullptr, BuildingRef))
					{
						PlayerStateRef->SetUnits(GetPlayerState<ACustomPlayerState>()->GetUnits() + 1);
						PlayerAction = EDC_ActionPlayer::None;
						break;
					}
				}
			}
			PlayerAction = EDC_ActionPlayer::None;
			break;
		case EDC_ActionPlayer::SpellCast:
			if(UnitRef)
			{
				if(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile != nullptr)
				{
					if(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile->GetPlayerOwner() != UnitRef->GetPlayerOwner())
					{
						UnitRef->PrepareSpecial(PlayerPositionInGrid);
						AllPlayerActions.Add(FStructActions(UnitRef, EDC_ActionPlayer::SpellCast));
					}
					if (Grid->GetGridData()->Find(PlayerPositionInGrid)->BaseOnTile)
					{
						UnitRef->PrepareSpecial(PlayerPositionInGrid);
						AllPlayerActions.Add(FStructActions(UnitRef, EDC_ActionPlayer::SpellCast));
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
		default:
			break;
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

void ACustomPlayerController::AttackBase_Implementation(ABase* BaseToAttack, AUnit* UnitAttacking)
{
	if(BaseToAttack && UnitAttacking)
		UnitAttacking->AttackBase(BaseToAttack);
	else
	{
		ActionEndTurn();
	}
}

void ACustomPlayerController::ServerAttackUnit_Implementation(AUnit* UnitToAttack, AUnit* UnitAttacking)
{
	if(UnitToAttack && UnitAttacking)
		UnitToAttack->AttackUnit(UnitAttacking);
	else
	{
		ActionEndTurn();
	}
}

void ACustomPlayerController::ServerAttackBuilding_Implementation(ATower* TowerToAttack, AUnit* UnitAttacking)
{
	if(TowerToAttack && UnitAttacking)
		TowerToAttack->AttackUnit(UnitAttacking,this);
	else
	{
		ActionEndTurn();
	}
}

bool ACustomPlayerController::SpawnUnit(EUnitType UnitToSpawn, FIntPoint SpawnChosen,ABase* BaseToSpawn, ABuilding* BuildingToSpawn)
{
	Server_SpawnUnit(UnitToSpawn, SpawnChosen, BaseToSpawn, BuildingToSpawn);
	return Grid->GetGridData()->Find(SpawnChosen)->UnitOnTile != nullptr;
}

void ACustomPlayerController::Multicast_SpawnUnit_Implementation(AUnit* UnitSpawned,AGrid* GridSpawned, ACustomPlayerState* PlayerStatRef,  ABase* BaseSpawned, ABuilding* BuildingSpawned)
{
	if(UnitSpawned)
	{
		UnitSpawned->Grid = Grid;
		
		if(BuildingSpawned)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("BuildingSpawned"));
			UnitSpawned->SetPlayerOwner(BuildingSpawned->PlayerOwner);
			BuildingRef->BuildingPreAction(UnitSpawned);
			AllPlayerPassive.Add(FStructPassive(BuildingRef, EDC_ActionPlayer::SelectBuilding));
		}
			
		if(BaseSpawned)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("BaseSpawned"));
			UnitSpawned->SetPlayerOwner(BaseSpawned->PlayerOwner);
			BaseSpawned->BasePreAction(UnitSpawned);
			AllPlayerPassive.Add(FStructPassive(BaseSpawned, EDC_ActionPlayer::SelectBuilding));
		}

	}
}

void ACustomPlayerController::Server_SpawnUnit_Implementation(EUnitType UnitToSpawn, FIntPoint SpawnChosen , ABase* BaseToSpawn, ABuilding* BuildingToSpawn)
{

	TArray<int> CostOfSpawn;
	switch (UnitToSpawn)
	{
	case EUnitType::U_Warrior:
		CostOfSpawn = {0,10,20};
		break;
	case EUnitType::U_Tank:
		CostOfSpawn = {10,20,0};
		break;
	case EUnitType::U_Mage:
		CostOfSpawn = {20,0,10};
		break;
	case EUnitType::U_Leader:
		CostOfSpawn = {20,20,20};
		break;
	default:
		CostOfSpawn = {15,15,15};
	}
	if(!PlayerStateRef)
		PlayerStateRef = Cast<ACustomPlayerState>(PlayerState);
	
	if (Grid->GetGridData()->Find(SpawnChosen)->UnitOnTile == nullptr
		&& (PlayerStateRef->GetWoodPoints() >= CostOfSpawn[0])
		&& (PlayerStateRef->GetStonePoints() >= CostOfSpawn[1])
		&& (PlayerStateRef->GetGoldPoints() >= CostOfSpawn[2]))
	{
		PlayerStateRef->ChangeWoodPoints(CostOfSpawn[0], false);
		PlayerStateRef->ChangeStonePoints(CostOfSpawn[1], false);
		PlayerStateRef->ChangeGoldPoints(CostOfSpawn[2], false);
		AUnit* UnitThatSpawned;
		switch (UnitToSpawn)
		{
		case EUnitType::U_Warrior:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Warrior>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			Multicast_SpawnUnit(UnitThatSpawned,Grid,PlayerStateRef, BaseToSpawn, BuildingToSpawn);
			
			break;
		case EUnitType::U_Mage:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Mage>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			Multicast_SpawnUnit(UnitThatSpawned,Grid,PlayerStateRef, BaseToSpawn, BuildingToSpawn);
			break;
		case EUnitType::U_Tank:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Tank>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			Multicast_SpawnUnit(UnitThatSpawned,Grid,PlayerStateRef, BaseToSpawn, BuildingToSpawn);
			break;
		case EUnitType::U_Leader:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Leader>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			Multicast_SpawnUnit(UnitThatSpawned,Grid,PlayerStateRef, BaseToSpawn, BuildingToSpawn);
			break;
		default:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Warrior>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			Multicast_SpawnUnit(UnitThatSpawned,Grid,PlayerStateRef, BaseToSpawn, BuildingToSpawn);
			break;
		}
	}
}

void ACustomPlayerController::EndTurn()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("EndTurn"));
	Server_EndTurn();
}

void ACustomPlayerController::ActionEndTurn()
{
	GEngine->AddOnScreenDebugMessage( -1,5.f,FColor::Emerald,TEXT("ActionEndTurn"));
	if(IsLocalController())
	{
		if(PlayerStateRef == nullptr)
			PlayerStateRef = Cast<ACustomPlayerState>(PlayerState);

		if(!PlayerStateRef->bIsActiveTurn)
		{
			FTimerHandle TimerActiveEndTurn;
			
			if(AllPlayerActions.Num() > 0)
			{
				AUnit* UnitAction = Cast<AUnit>( AllPlayerActions[0].ActorRef);
				ATower* TowerAction = Cast<ATower>( AllPlayerActions[0].ActorRef);
				ABase* BaseAction = Cast<ABase>( AllPlayerActions[0].ActorRef);
				if(UnitAction)
				{
					UnitAction->HasActed = false;
					UnitAction->HasMoved = false;
				}
				switch (AllPlayerActions[0].UnitAction)
				{
				case EDC_ActionPlayer::MoveUnit:
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("MoveUnit"));
					AllPlayerActions.RemoveAt(0);
					ServerMoveUnit(UnitAction);
					break;
				case EDC_ActionPlayer::AttackUnit:
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("AttackUnit"));
					ServerAttackUnit(UnitAction, AllPlayerActions[0].UnitAttacking);
					GetWorld()->GetTimerManager().SetTimer(TimerActiveEndTurn, this, &ACustomPlayerController::ActionEndTurn, 0.5f, false);
					AllPlayerActions.RemoveAt(0);
					
					break;
				case EDC_ActionPlayer::AttackBuilding:
					if(TowerAction)
					{
						UE_LOG( LogTemp, Warning, TEXT("TowerRef") );
						ServerAttackBuilding(TowerAction, AllPlayerActions[0].UnitAttacking);
						GetWorld()->GetTimerManager().SetTimer(TimerActiveEndTurn, this, &ACustomPlayerController::ActionEndTurn, 0.5f, false);
						TowerAction = nullptr;
					}
					if(BaseAction)
                    {
                        UE_LOG( LogTemp, Warning, TEXT("BaseRef") );
                        AttackBase(BaseAction, AllPlayerActions[0].UnitAttacking);
                        GetWorld()->GetTimerManager().SetTimer(TimerActiveEndTurn, this, &ACustomPlayerController::ActionEndTurn, 0.5f, false);
                    }
					AllPlayerActions.RemoveAt(0);
					break;
				case EDC_ActionPlayer::SpellCast:
                        AllPlayerActions.RemoveAt(0);
                        UnitAction->Special();
                        break;
				default:
					break;
				}
			}else
			{
				AllPlayerActions.Empty();
				GetWorld()->GetTimerManager().ClearTimer(TimerActiveEndTurn);
			}
		}
		else
		{
			if(!AllPlayerPassive.IsEmpty())
			{
				if(ABuilding *BuildingAction = Cast<ABuilding>(AllPlayerPassive[0].ActorRef))
                {
                    BuildingAction->BuildingAction();
                    AllPlayerPassive.RemoveAt(0);
                }
				else if(ABase *BaseAction = Cast<ABase>(AllPlayerPassive[0].ActorRef))
                {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("BaseAction"));
                    BaseAction->BaseAction();
                    AllPlayerPassive.RemoveAt(0);
                }
			}
		}
	}

}

void ACustomPlayerController::UpdateUITimer_Implementation(int TimeLeft)
{
}

void ACustomPlayerController::UpdateWidget3D_Implementation(bool bInteractive, bool bVisibility)
{
}


// Display Widget 3D on camera depending if item is interactive or not

void ACustomPlayerController::VerifyBuildInteraction()
{
	if (PlayerStateRef == nullptr)
    {
        PlayerStateRef = Cast<ACustomPlayerState>(PlayerState);
    }
	
	if (PlayerAction == EDC_ActionPlayer::None)
	{
		if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->UnitOnTile != nullptr)
		{
			if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->UnitOnTile->GetPlayerOwner() == PlayerStateRef->PlayerTeam && PlayerStateRef->bIsActiveTurn)
			{
				UpdateWidget3D(true, true);
			}
			else UpdateWidget3D(false, true);
		}

		else if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->BaseOnTile != nullptr)
		{
				if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->BaseOnTile->PlayerOwner == PlayerStateRef->PlayerTeam && !PlayerStateRef->bIsActiveTurn)
			{
				UpdateWidget3D(true, true);
			}
			else UpdateWidget3D(false, true);
		}

		else if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->TowerOnTile != nullptr)
		{
			if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->TowerOnTile->GetPlayerOwner() == PlayerStateRef->PlayerTeam && PlayerStateRef->bIsActiveTurn)
			{
				UpdateWidget3D(true, true);
			}
			else UpdateWidget3D(false, true);
		}

		else UpdateWidget3D(false, false);
	}
	else UpdateWidget3D(false, false);
}

void ACustomPlayerController::AssignPlayerPosition()
{
	if(!PlayerStateRef)
		PlayerStateRef = Cast<ACustomPlayerState>(PlayerState);
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADC_CustomPlayerStart::StaticClass(), FoundActors);
	for(AActor* CurrentActor : FoundActors)
	{
		if(Cast <ADC_CustomPlayerStart>(CurrentActor))
        {
			ADC_CustomPlayerStart* PlayerStart = Cast<ADC_CustomPlayerStart>(CurrentActor);
			if(PlayerStateRef)
				if(PlayerStart->PlayerTeam == PlayerStateRef->PlayerTeam)
				{
					if(CameraPlayerRef)
					{
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("PlayerStart Position : ") + PlayerStart->GetActorLocation().ToString());
						CameraPlayerRef->SetActorLocation(PlayerStart->GetActorLocation());
						CameraPlayerRef->FullMoveDirection = PlayerStart->GetActorLocation();
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("CameraPlayerRef Position : ") + CameraPlayerRef->GetActorLocation().ToString());
						return;
					}

				}
        }
	}
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACustomPlayerController::AssignPlayerPosition, 0.2f, false);
}

void ACustomPlayerController::UpdateUi_Implementation()
{
}

void ACustomPlayerController::Server_SpawnBaseUnit_Implementation(EUnitType UnitToSpawn,AGrid* GridSer, ABase* BaseToSpawn, EPlayer PlayerOwner)
{
	GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Green, TEXT("BaseToSpawn : " + BaseToSpawn->GetName()));
	if (GridSer != nullptr)
	{
		FIntPoint SpawnLoc = FIntPoint(-999,-999);
		for (FIntPoint Index : BaseToSpawn->AllSpawnLoc)
		{
			if (GridSer->GetGridData()->Find(Index)->UnitOnTile == nullptr)
			{
				SpawnLoc = Index;
				break;
			}
		}
		if(SpawnLoc != FIntPoint(-999,-999))
		{
			SpawnUnit(UnitToSpawn, SpawnLoc, BaseToSpawn, nullptr);
			GridSer->GridVisual->RemoveStateFromTile(SpawnLoc, EDC_TileState::Spawnable);
		}
	}
}

void ACustomPlayerController::ServerMoveUnit_Implementation(const AUnit* UnitToMove)
{
	AUnit* RefUnit = const_cast<AUnit*>(UnitToMove);
	
	if(UnitToMove)
	{
		RefUnit->MoveUnitEndTurn();
		UnitRef = nullptr;
	}
}

void ACustomPlayerController::SpawnBaseUnit(EUnitType UnitToSpawn)
{
	UE_LOG(	LogTemp, Warning, TEXT("SpawnBaseUnit") );
	Server_SpawnBaseUnit(UnitToSpawn, Grid, BaseRef, PlayerTeam);
}

void ACustomPlayerController::Server_PrepareMoveUnit_Implementation(const TArray<FIntPoint>& Path,
	const AUnit* UnitToMove)
{
	AUnit *UnitRefServer = const_cast<AUnit*>(UnitToMove);
	
	UnitRefServer->Multi_PrepareMove(Path);
		
	
}



void ACustomPlayerController::Server_EndTurn_Implementation()
{
	PlayerStateRef->SetIsReadyToSwitchTurn(true);
	Cast<ACustomGameState>(GetWorld()->GetGameState())->CheckSwitchPlayerTurn();
}
/*void ACustomPlayerController::DisplayWidgetEndGame_Implementation()
{
}*/

void ACustomPlayerController::DisplayWidget_Implementation()
{
}

void ACustomPlayerController::SelectModeSelectBuilding()
{
	PlayerAction = EDC_ActionPlayer::SelectBuilding;
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

void ACustomPlayerController::SetPlayerAction(EDC_ActionPlayer PA)
{
	PlayerAction = PA;
}

