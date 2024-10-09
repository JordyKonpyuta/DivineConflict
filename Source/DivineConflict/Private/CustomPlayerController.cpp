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
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACustomPlayerController::AssignPlayerPosition, 2.2f, false);
}

void ACustomPlayerController::GetLifetimeReplicatedProps( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACustomPlayerController, PlayerTeam);

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
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("OnRep_PlayerTeam"));
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
					GEngine->AddOnScreenDebugMessage(-1, 5.f,FColor::Blue,TEXT("ActifTurn"));
					if(Grid->GetGridData()->Find(PlayerPositionInGrid)->TowerOnTile)
					{
						if(Grid->GetGridData()->Find(PlayerPositionInGrid)->TowerOnTile->GetPlayerOwner() == PlayerStateRef->PlayerTeam)
						{
							TowerRef = Grid->GetGridData()->Find(PlayerPositionInGrid)->TowerOnTile;
							TowerRef->IsSelected = true;
							DisplayWidgetTower();
						}
					}
						

					else if (Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile)
					{
						if(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile->GetPlayerOwner() == PlayerStateRef->PlayerTeam)
						{
							UnitRef = Grid->GridData.Find(PlayerPositionInGrid)->UnitOnTile;
							CameraPlayerRef->SetCustomPlayerController(this);
							IInteractInterface::Execute_Interact(Grid->GridData.Find(PlayerPositionInGrid)->UnitOnTile, this);
							DisplayWidget();
						}
					}
				}
				else
				{
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
					TowerRef->AttackUnit(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile);
					//AllPlayerActions.Add(FStructActions(UnitRef, EDC_ActionPlayer::AttackUnit));
				}
			}
			break;
		case EDC_ActionPlayer::AttackUnit:
			if (UnitRef)
			{
				// Are we attacking a Unit?
				if(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile)
				{
					if (Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile->GetPlayerOwner() != PlayerStateRef->PlayerTeam)
					{
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("AttackUnit"));
						UnitRef->PrepareAttackUnit(PlayerPositionInGrid);
						AllPlayerActions.Add(FStructActions(UnitRef, EDC_ActionPlayer::AttackUnit));
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
							AllPlayerActions.Add(FStructActions(UnitRef, EDC_ActionPlayer::AttackUnit));
						}
					}
				}
				// Are we attacking a Base?
				else if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->BaseOnTile)
				{
					if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->BaseOnTile->PlayerOwner != UnitRef->GetPlayerOwner())
					{
						UnitRef->AttackBase(Grid->GetGridData()->Find(PlayerPositionInGrid)->BaseOnTile);
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
		case EDC_ActionPlayer::MoveUnit:
			if (UnitRef)
			{
				for(FIntPoint Index : PathReachable)
				{
					Grid->GridVisual->RemoveStateFromTile(Index, EDC_TileState::Reachable);
				}
				UnitRef->SetIsSelected(false);
				Server_PrepareMoveUnit(CameraPlayerRef->Path,UnitRef);
				PathReachable.Empty();
				CameraPlayerRef->IsMovingUnit = false;
				CameraPlayerRef->Path.Empty();
				AllPlayerActions.Add(FStructActions(UnitRef, EDC_ActionPlayer::MoveUnit));
				UpdateUi();
					
				PlayerAction = EDC_ActionPlayer::None;
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
					if (SpawnUnit(BuildingRef->UnitProduced, SpawnIndex))
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
			break;
		default:
			break;
		}
	}
}

/*
void ACustomPlayerController::ControllerInteraction()
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
				}
				break;

}*/

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
	Server_SpawnUnit(UnitToSpawn, SpawnChosen);
	return Grid->GetGridData()->Find(SpawnChosen)->UnitOnTile != nullptr;
}

void ACustomPlayerController::Multicast_SpawnUnit_Implementation(AUnit* UnitSpawned,AGrid* GridSpawned, ACustomPlayerState* PlayerStatRef)
{
	if(UnitSpawned)
	{
		UnitSpawned->Grid = Grid;
		UnitSpawned->SetPlayerOwner(PlayerStateRef->PlayerTeam);
	}
		
	
}

void ACustomPlayerController::Server_SpawnUnit_Implementation(EUnitType UnitToSpawn, FIntPoint SpawnChosen)
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
			Multicast_SpawnUnit(UnitThatSpawned,Grid,PlayerStateRef);
			
			break;
		case EUnitType::U_Mage:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Mage>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			Multicast_SpawnUnit(UnitThatSpawned,Grid,PlayerStateRef);
			break;
		case EUnitType::U_Tank:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Tank>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			Multicast_SpawnUnit(UnitThatSpawned,Grid,PlayerStateRef);
			break;
		case EUnitType::U_Leader:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Leader>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			Multicast_SpawnUnit(UnitThatSpawned,Grid,PlayerStateRef);
			break;
		default:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Warrior>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			Multicast_SpawnUnit(UnitThatSpawned,Grid,PlayerStateRef);
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

	if(IsLocalController())
	{
		if(PlayerStateRef == nullptr)
			PlayerStateRef = Cast<ACustomPlayerState>(PlayerState);

		if(!PlayerStateRef->bIsActiveTurn)
		{
			
			if(AllPlayerActions.Num() > 0)
			{
				AUnit* UnitAction = AllPlayerActions[0].Unit;
				switch (AllPlayerActions[0].UnitAction)
				{
				case EDC_ActionPlayer::MoveUnit:
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("MoveUnit"));
					AllPlayerActions.RemoveAt(0);
					ServerMoveUnit(UnitAction);
					break;
				case EDC_ActionPlayer::AttackUnit:
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("AttackUnit"));
					AllPlayerActions.RemoveAt(0);
					UnitAction->AttackUnit();				
					break;
				default:
					break;
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
	if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->UnitOnTile != nullptr)
	{
		if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->UnitOnTile->GetPlayerOwner() == PlayerTeam && PlayerStateRef->bIsActiveTurn)
		{
			UpdateWidget3D(true, true);
		}
		else UpdateWidget3D(false, true);
	}

	else if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->BaseOnTile != nullptr)
	{
		if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->BaseOnTile->PlayerOwner == PlayerTeam && !PlayerStateRef->bIsActiveTurn)
		{
			UpdateWidget3D(true, true);
		}
		else UpdateWidget3D(false, true);
	}

	else if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->TowerOnTile != nullptr)
	{
		if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->TowerOnTile->GetPlayerOwner() == PlayerTeam && PlayerStateRef->bIsActiveTurn)
		{
			UpdateWidget3D(true, true);
		}
		else UpdateWidget3D(false, true);
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
	UE_LOG( LogTemp, Warning, TEXT("Server_SpawnBaseUnit") );
	if (GridSer != nullptr)
	{
		UE_LOG( LogTemp, Warning, TEXT("Server_SpawnBaseUnit") );
		UE_LOG( LogTemp, Warning, TEXT("AllSpawnLoc: %d"), BaseToSpawn->AllSpawnLoc.Num() );
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
			SpawnUnit(UnitToSpawn, SpawnLoc);
			GridSer->GetGridData()->Find(SpawnLoc)->UnitOnTile->SetPlayerOwner(PlayerOwner);
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
	if(UnitRefServer)
	{
		UnitRefServer->Multi_PrepareMove(Path);
		
	}
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

