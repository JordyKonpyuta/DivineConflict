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
#include "Unit/GhostUnitSpawning.h"
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
			AUnit_Child_Warrior* TempWarriorRef = Cast<AUnit_Child_Warrior>(UnitRef);
			if (UnitRef->HasActed && TempWarriorRef && UnitRef->GetFinalGhostMesh()->IsVisible())
			{
				PathReachable = Grid->GridPath->NewFindPath(Grid->ConvertLocationToIndex(UnitRef->GetFinalGhostMesh()->GetComponentLocation()), this);
			}
			else
				PathReachable = Grid->GridPath->NewFindPath(Grid->ConvertLocationToIndex(UnitRef->GetActorLocation()), this);

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
	if (UnitRef)
	{
		if (Cast<AUnit_Child_Warrior>(UnitRef))
		{
			if (UnitRef->HasActed &&  UnitRef->GetFinalGhostMesh()->IsVisible())
			{
				CameraPlayerRef->Path.Add(Grid->ConvertLocationToIndex(UnitRef->GetFinalGhostMesh()->GetComponentLocation()));
				CameraPlayerRef->FullMoveDirection.X = UnitRef->GetFinalGhostMesh()->GetComponentLocation().X;
				CameraPlayerRef->FullMoveDirection.Y = UnitRef->GetFinalGhostMesh()->GetComponentLocation().Y;
				CameraPlayerRef->FullMoveDirection.Z = (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 175;
			}
			else
				CameraPlayerRef->Path.Add(UnitRef->GetIndexPosition());
		}
		else
			CameraPlayerRef->Path.Add(UnitRef->GetIndexPosition());
	}
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
			//AllPlayerActions.Add(FStructActions(UnitRef, EDC_ActionPlayer::Special));
			UnitRef->Special();
		AllPlayerActions.Add(FStructActions(UnitRef, EDC_ActionPlayer::MoveUnit));
		UnitRef->HasActed = true;
		break;
	case EUnitName::Tank:
		//Tank
			AllPlayerActions.Add(FStructActions(UnitRef, EDC_ActionPlayer::Special));
		UnitRef->HasActed = true;
		break;
	case EUnitName::Mage:
		//Mage
			if (CameraPlayerRef)
				CameraPlayerRef->IsSpelling = true;
			if (UnitRef->HasMoved)
			{
				PathReachable =  Grid->GridPath->FindPath(Grid->ConvertLocationToIndex(UnitRef->GetFinalGhostMesh()->GetComponentLocation()),
					FIntPoint(-999,-999),true,4,false);
				CameraPlayerRef->FullMoveDirection.X = UnitRef->GetFinalGhostMesh()->GetComponentLocation().X;
				CameraPlayerRef->FullMoveDirection.Y = UnitRef->GetFinalGhostMesh()->GetComponentLocation().Y;
				CameraPlayerRef->FullMoveDirection.Z = (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 175;
			} else
			{
				PathReachable =  Grid->GridPath->FindPath(UnitRef->GetIndexPosition(),FIntPoint(-999,-999),true,4,false);
			}
		for(FIntPoint Index : PathReachable)
		{
			Grid->GridVisual->addStateToTile(Index, EDC_TileState::Attacked);
		}
		PlayerAction = EDC_ActionPlayer::Special;
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
	CameraPlayerRef->IsTowering = true;
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
					TowerRef->PreprareAttack(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile);
					AllPlayerActions.Add(FStructActions(TowerRef, EDC_ActionPlayer::AttackBuilding, Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile));
					PlayerAction = EDC_ActionPlayer::None;
					DisplayWidget();
				}
				CameraPlayerRef->IsTowering = false;
			}
			break;
		case EDC_ActionPlayer::AttackUnit:
			if (UnitRef)
			{
				if (!UnitRef->HasActed && UnitRef->GetIndexPosition() != PlayerPositionInGrid){
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
							if(Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->BuildingOnTile->GarrisonFull)
							{
								//UnitRef->PrepareAttackBuilding(PlayerPositionInGrid);
								AllPlayerActions.Add(FStructActions(Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile, EDC_ActionPlayer::AttackBuilding, UnitRef));
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
					else
					{
						AllPlayerActions.Add(FStructActions(UnitRef, EDC_ActionPlayer::AttackUnit, nullptr));
					}
					for(FIntPoint Index : PathReachable)
					{
						Grid->GridVisual->RemoveStateFromTile(Index, EDC_TileState::Attacked);
					}
					PathReachable.Empty();
					UnitRef->SetIsSelected(false);
					UnitRef->HasActed = true;
					CameraPlayerRef->IsAttacking = false;
					PlayerAction = EDC_ActionPlayer::None;
					DisplayWidget();
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
					
					PlayerAction = EDC_ActionPlayer::None;
					DisplayWidget();
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
					SpawnGhostUnit(BuildingRef->UnitProduced, SpawnIndex);
					PlayerAction = EDC_ActionPlayer::None;
					break;
				}
			}
			PlayerAction = EDC_ActionPlayer::None;
			break;
		case EDC_ActionPlayer::Special:
			if(UnitRef)
			{
				if(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile != nullptr)
				{
					if(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile->GetPlayerOwner() != UnitRef->GetPlayerOwner())
					{
						UnitRef->PrepareSpecial(PlayerPositionInGrid);
						AllPlayerActions.Add(FStructActions(UnitRef, EDC_ActionPlayer::Special, Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile));
					}
					if (Grid->GetGridData()->Find(PlayerPositionInGrid)->BaseOnTile)
					{
						UnitRef->PrepareSpecial(PlayerPositionInGrid);
						AllPlayerActions.Add(FStructActions(Grid->GetGridData()->Find(PlayerPositionInGrid)->BaseOnTile, EDC_ActionPlayer::Special, UnitRef));
					}
				}
				for(FIntPoint Index : PathReachable)
				{
					Grid->GridVisual->RemoveStateFromTile(Index, EDC_TileState::Attacked);
				}
				PathReachable.Empty();
				UnitRef->SetIsSelected(false);
				PlayerAction = EDC_ActionPlayer::None;
				DisplayWidget();
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

void ACustomPlayerController::HiddeWidget_Implementation()
{
}

void ACustomPlayerController::FeedbackEndTurn_Implementation(bool visibility)
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
	{
		UnitAttacking->AttackBase(BaseToAttack);
		UnitAttacking->AnimAttack(BaseToAttack);
	}
	else
	{
		Multi_ActionActiveTurn();
	}
}

void ACustomPlayerController::ServerAttackUnit_Implementation(AUnit* UnitToAttack, AUnit* UnitAttacking)
{
	if(UnitToAttack && UnitAttacking)
	{
		UnitAttacking->UnitToAttackRef = UnitToAttack;
		UnitToAttack->AnimAttack(UnitAttacking);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Silver,TEXT("Active"));
		//Multi_ActionActiveTurn();
	}
}

void ACustomPlayerController::ServerAttackBuilding_Implementation(ABuilding* BuildingToAttack, AUnit* UnitAttacking)
{
	if(BuildingToAttack && UnitAttacking)
		UnitAttacking->AttackBuilding(BuildingToAttack);
	else
	{
		Multi_ActionActiveTurn();
	}
}

void ACustomPlayerController::ServerAttackTower_Implementation(ATower* TowerToAttack, AUnit* UnitAttacking)
{
	if(TowerToAttack && UnitAttacking)
	{
		TowerToAttack->AttackUnit(UnitAttacking,this);
	}
	else
	{
		Multi_ActionActiveTurn();
	}
}

void ACustomPlayerController::Server_SpecialUnit_Implementation(AUnit* UnitSpecial,AActor* ThingToAttack)
{
	if(AUnit_Child_Mage* MageSp =  Cast<AUnit_Child_Mage>(UnitSpecial))
		MageSp->SpecialMage(ThingToAttack);
	else UnitSpecial->Special();
}

bool ACustomPlayerController::SpawnUnit(EUnitType UnitToSpawn, FIntPoint SpawnChosen,ABase* BaseToSpawn, ABuilding* BuildingToSpawn)
{
	Server_SpawnUnit(UnitToSpawn, SpawnChosen, BaseToSpawn, BuildingToSpawn,PlayerStateRef);
	return Grid->GetGridData()->Find(SpawnChosen)->UnitOnTile != nullptr;
}

void ACustomPlayerController::Multicast_SpawnUnit_Implementation(AUnit* UnitSpawned,AGrid* GridSpawned, ACustomPlayerState* PlayerStatRef,  ABase* BaseSpawned, ABuilding* BuildingSpawned)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Multicast_SpawnUnit"));
	if(UnitSpawned)
	{
		UnitSpawned->Grid = Grid;		
		if(BuildingSpawned)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("BuildingSpawned"));
			UnitSpawned->SetPlayerOwner(BuildingSpawned->PlayerOwner);
		}
			
		if(BaseSpawned)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("BaseSpawned"));
			UnitSpawned->SetPlayerOwner(BaseSpawned->PlayerOwner);
		}
	}
	Client_UpdateUi();
}

void ACustomPlayerController::Server_SpawnUnit_Implementation(EUnitType UnitToSpawn, FIntPoint SpawnChosen , ABase* BaseToSpawn, ABuilding* BuildingToSpawn, ACustomPlayerState* PlayerStatRef)
{
	
	if (Grid->GetGridData()->Find(SpawnChosen)->UnitOnTile == nullptr && PlayerStatRef->CurrentUnitCount <= PlayerStatRef->MaxUnitCount)
	{
	
		AUnit* UnitThatSpawned;
		switch (UnitToSpawn)
		{
		case EUnitType::U_Warrior:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Warrior>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			Multicast_SpawnUnit(UnitThatSpawned,Grid,PlayerStatRef, BaseToSpawn, BuildingToSpawn);
			PlayerStatRef->SetWarriorCount(PlayerStatRef->GetWarriorCount() + 1);
			break;
		case EUnitType::U_Mage:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Mage>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			Multicast_SpawnUnit(UnitThatSpawned,Grid,PlayerStatRef, BaseToSpawn, BuildingToSpawn);
			PlayerStatRef->SetMageCount(PlayerStatRef->GetMageCount() + 1);
			break;
		case EUnitType::U_Tank:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Tank>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			Multicast_SpawnUnit(UnitThatSpawned,Grid,PlayerStatRef, BaseToSpawn, BuildingToSpawn);
			PlayerStatRef->SetTankCount(PlayerStatRef->GetTankCount() + 1);
			break;
		case EUnitType::U_Leader:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Leader>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			Multicast_SpawnUnit(UnitThatSpawned,Grid,PlayerStatRef, BaseToSpawn, BuildingToSpawn);
			PlayerStatRef->SetLeaderCount(PlayerStatRef->GetLeaderCount() + 1);
			break;
		default:
			UnitThatSpawned = GetWorld()->SpawnActor<AUnit_Child_Warrior>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
			Multicast_SpawnUnit(UnitThatSpawned,Grid,PlayerStatRef, BaseToSpawn, BuildingToSpawn);
			PlayerStatRef->SetWarriorCount(PlayerStatRef->GetWarriorCount() + 1);
			break;
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("UnitCount : " + FString::FromInt(PlayerStatRef->CurrentUnitCount) + " WarriorCount : " + FString::FromInt(PlayerStatRef->GetWarriorCount())
		+"TankCount : " + FString::FromInt(PlayerStatRef->GetTankCount()) +"MageCount : " + FString::FromInt(PlayerStatRef->GetMageCount()) +"LeaderCount : " + FString::FromInt(PlayerStatRef->GetLeaderCount())));
}

void ACustomPlayerController::EndTurn()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("EndTurn"));
	Server_EndTurn();
	FeedbackEndTurn(true);
}

void ACustomPlayerController::Server_ActionActiveTurn_Implementation()
{
	Multi_ActionActiveTurn();
}

void ACustomPlayerController::Multi_ActionActiveTurn_Implementation()
{
	if(IsLocalController())
	{
		HiddeWidget();
		if(PlayerStateRef == nullptr)
			PlayerStateRef = Cast<ACustomPlayerState>(PlayerState);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Multi_ActionActiveTurn"));
		GetWorld()->GetTimerManager().ClearTimer(TimerActiveEndTurn);
		if(PlayerStateRef->bIsActiveTurn)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("IsActiveTurn"));
			
			GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Silver, TEXT("AllPlayerAction : " + FString::FromInt(AllPlayerActions.Num())));
			if(AllPlayerActions.Num() > 0)
			{
				AUnit* UnitAction = Cast<AUnit>( AllPlayerActions[0].ActorRef);
				ATower* TowerAction = Cast<ATower>( AllPlayerActions[0].ActorRef);
				ABuilding* BuildingAction = Cast<ABuilding>( AllPlayerActions[0].ActorRef);
				ABase* BaseAction = Cast<ABase>( AllPlayerActions[0].ActorRef);
				if(UnitAction)
				{
					UnitAction->HasActed = false;
					UnitAction->HasMoved = false;
				}
				if(BaseAction || BuildingAction)
				{
					AllPlayerActions[0].UnitAttacking->HasActed = false;
					AllPlayerActions[0].UnitAttacking->HasMoved = false;
				}
				if (TowerAction)
				{
					TowerAction->SetCanAttack(true);
				}
				switch (AllPlayerActions[0].UnitAction)
				{
				case EDC_ActionPlayer::MoveUnit:
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("MoveUnit"));
					ServerMoveUnit(UnitAction);
					AllPlayerActions.RemoveAt(0);
					break;
				case EDC_ActionPlayer::AttackUnit:
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("AttackUnit"));
					ServerAttackUnit(UnitAction, AllPlayerActions[0].UnitAttacking);
					GetWorld()->GetTimerManager().SetTimer(TimerActiveEndTurn, this, &ACustomPlayerController::Multi_ActionActiveTurn, 0.5f, false);
					AllPlayerActions.RemoveAt(0);
					break;
				case EDC_ActionPlayer::AttackBuilding:
					if(TowerAction)
					{
						UE_LOG( LogTemp, Warning, TEXT("TowerRef") );
						ServerAttackTower(TowerAction, AllPlayerActions[0].UnitAttacking);
						GetWorld()->GetTimerManager().SetTimer(TimerActiveEndTurn, this, &ACustomPlayerController::Multi_ActionActiveTurn, 0.5f, false);
						TowerAction = nullptr;
					}
					if(BaseAction)
					{
						UE_LOG( LogTemp, Warning, TEXT("BaseRef") );
						AttackBase(BaseAction, AllPlayerActions[0].UnitAttacking);
						GetWorld()->GetTimerManager().SetTimer(TimerActiveEndTurn, this, &ACustomPlayerController::Multi_ActionActiveTurn, 0.5f, false);
					}
					if (BuildingAction)
					{
						UE_LOG( LogTemp, Warning, TEXT("BuildingRef") );
						ServerAttackBuilding(BuildingAction, AllPlayerActions[0].UnitAttacking);
						GetWorld()->GetTimerManager().SetTimer(TimerActiveEndTurn, this, &ACustomPlayerController::Multi_ActionActiveTurn, 0.5f, false);
						BuildingAction = nullptr;
					}
					AllPlayerActions.RemoveAt(0);
					break;
				case EDC_ActionPlayer::Special:
					if(BaseAction)
						Server_SpecialUnit(AllPlayerActions[0].UnitAttacking, BaseAction);
					if(UnitAction)
						Server_SpecialUnit(UnitAction,AllPlayerActions[0].UnitAttacking);
					GetWorld()->GetTimerManager().SetTimer(TimerActiveEndTurn, this, &ACustomPlayerController::Multi_ActionActiveTurn, 0.5f, false);
					AllPlayerActions.RemoveAt(0);
					break;
				default:
					break;
				}
			}else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, TEXT("EndActionActiveTurn"));
				AllPlayerActions.Empty();
				GetWorld()->GetTimerManager().ClearTimer(TimerActiveEndTurn);
				Server_CheckPlayerActionPassive();
			}
		}
	}
}

EDC_ActionPlayer ACustomPlayerController::GetPlayerAction()
{
	return PlayerAction;
}

void ACustomPlayerController::Server_CheckPlayerActionPassive_Implementation()
{
	Cast<ACustomGameState>(GetWorld()->GetGameState())->CheckPlayerActionPassive();
}


void ACustomPlayerController::Server_ActionPassiveTurn_Implementation()
{
	Multi_ActionPassiveTurn();
}

void ACustomPlayerController::Multi_ActionPassiveTurn_Implementation()
{
	if(IsLocalController())
	{
		if(!AllPlayerPassive.IsEmpty())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("AllPlayerPassive"));
			if(ABuilding *BuildingAction = Cast<ABuilding>(AllPlayerPassive[0].ActorRef))
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("BuildingAction"));
				GetWorld()->DestroyActor(AllPlayerPassive[0].GhostRef);
				Grid->GridVisual->RemoveStateFromTile(AllPlayerPassive[0].TilePosition, EDC_TileState::Spawned);
				Server_SpawnUnit(BuildingAction->UnitProduced, AllPlayerPassive[0].TilePosition, nullptr, BuildingAction, PlayerStateRef);
				AllPlayerPassive.RemoveAt(0);
				Multi_ActionPassiveTurn();
			}
			else if(ABase *BaseAction = Cast<ABase>(AllPlayerPassive[0].ActorRef))
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("BaseAction"));
				GetWorld()->DestroyActor(AllPlayerPassive[0].GhostRef);
				Grid->GridVisual->RemoveStateFromTile(AllPlayerPassive[0].TilePosition, EDC_TileState::Spawned);
				Server_SpawnBaseUnit(AllPlayerPassive[0].UnitType,Grid,BaseAction,PlayerStateRef->PlayerTeam);
				AllPlayerPassive.RemoveAt(0);
				Multi_ActionPassiveTurn();
			}
		}
		else
		{
			Server_SwitchPlayerTurn(Cast<ACustomGameState>(GetWorld()->GetGameState()));
			Server_Delegate();
		}
	}

}

void ACustomPlayerController::Server_SwitchPlayerTurn_Implementation(const ACustomGameState* GameState)
{
	Multi_SwitchPlayerTurn(GameState);
}

void ACustomPlayerController::Multi_SwitchPlayerTurn_Implementation(const ACustomGameState* GameState)
{
	ACustomGameState* GameStateRef = const_cast<ACustomGameState*>(GameState);
	if(GameStateRef)
	{
		GameStateRef->SwitchPlayerTurn();
	}
}


void ACustomPlayerController::Server_Delegate_Implementation()
{
	OnTurnChangedDelegate.Broadcast();
}

void ACustomPlayerController::CancelLastAction()
{
	if (!AllPlayerActions.IsEmpty() && PlayerStateRef->bIsActiveTurn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, TEXT("No :D"));
		AUnit* ActorThatStops = Cast<AUnit>(AllPlayerActions.Last().ActorRef);
		switch(AllPlayerActions.Last().UnitAction)
		{
			case EDC_ActionPlayer::MoveUnit:
				ActorThatStops->Server_CancelMove();
				break;
			case EDC_ActionPlayer::Special:
				ActorThatStops->Server_CancelSpecial();
				break;
			case EDC_ActionPlayer::AttackUnit:
				ActorThatStops->Server_CancelAttack();
				break;
			case EDC_ActionPlayer::AttackBuilding:
				ActorThatStops->Server_CancelAttack();
				break;
		default: break;
		}
		AllPlayerActions.RemoveAt(AllPlayerActions.Num() - 1);
		CurrentPA++;
	}
	else if (!AllPlayerPassive.IsEmpty() && !PlayerStateRef->bIsActiveTurn)
	{
		AllPlayerPassive.Last().GhostRef->Destroy();
		Grid->GridVisual->RemoveStateFromTile(AllPlayerPassive.Last().TilePosition, EDC_TileState::Spawned);
		
		AllPlayerPassive.RemoveAt(AllPlayerPassive.Num() - 1);
	}
}

void ACustomPlayerController::Client_PauseTimerUI_Implementation()
{
	PauseTimerUI();
}

void ACustomPlayerController::PauseTimerUI_Implementation()
{
}

void ACustomPlayerController::Client_UpdateUi_Implementation()
{
	UpdateUi();
}

void ACustomPlayerController::UpdateUITimer_Implementation(int TimeLeft)
{
}

void ACustomPlayerController::UpdateWidget3D_Implementation(int Index, bool bVisibility)
{
}


// Display Widget 3D on camera depending if item is interactive or not

void ACustomPlayerController::DisplayAttackWidget_Implementation()
{
}

void ACustomPlayerController::RemoveAttackWidget_Implementation()
{
}

void ACustomPlayerController::VerifyBuildInteraction()
{
	if (PlayerStateRef == nullptr)
    {
        PlayerStateRef = Cast<ACustomPlayerState>(PlayerState);
    }
	
	if (PlayerAction == EDC_ActionPlayer::None)
	{
		RemoveAttackWidget();
		
		if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->UnitOnTile != nullptr)
		{
			if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->UnitOnTile->GetPlayerOwner() == PlayerStateRef->PlayerTeam && PlayerStateRef->bIsActiveTurn)
			{
				UpdateWidget3D(0, true);
			}
			else UpdateWidget3D(1, true);
		}

		else if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->BaseOnTile != nullptr)
		{
				if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->BaseOnTile->PlayerOwner == PlayerStateRef->PlayerTeam && !PlayerStateRef->bIsActiveTurn)
			{
				UpdateWidget3D(0, true);
			}
			else UpdateWidget3D(1, true);
		}

		else if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->TowerOnTile != nullptr)
		{
			if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->TowerOnTile->GetPlayerOwner() == PlayerStateRef->PlayerTeam && PlayerStateRef->bIsActiveTurn)
			{
				UpdateWidget3D(0, true);
			}
			else UpdateWidget3D(1, true);
		}

		else UpdateWidget3D(1, false);
	}
	
	else if (PlayerAction == EDC_ActionPlayer::AttackUnit)
	{
		if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->UnitOnTile != nullptr)
		{
			if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->UnitOnTile->GetPlayerOwner() != PlayerStateRef->PlayerTeam)
			{
				Opponent = Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->UnitOnTile;
				DisplayAttackWidget();
			}
		}
		
		else if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->BuildingOnTile)
		{
			if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->BuildingOnTile->PlayerOwner != PlayerStateRef->PlayerTeam)
			{
				UpdateWidget3D(2, true);
			}
			else UpdateWidget3D(0, false);
		}

		else RemoveAttackWidget();
	}

	else
	{
		UpdateWidget3D(0, false);
	}
}

void ACustomPlayerController::AssignPlayerPosition()
{
	
	if(!PlayerStateRef)
	{
		PlayerStateRef = Cast<ACustomPlayerState>(PlayerState);
		PlayerStateRef->PlayerControllerRef = this;
	}
	UpdateUi();
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
			
			if (PlayerStateRef->PlayerTeam == EPlayer::P_Heaven)
			{
				UpdateWidget3D(0, true);
			}
			else if (PlayerStateRef->PlayerTeam == EPlayer::P_Hell)
			{
				UpdateWidget3D(1, true);
			}

			else UpdateWidget3D(0, true);
        }
	}
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACustomPlayerController::AssignPlayerPosition, 0.2f, false);
	
}

void ACustomPlayerController::SetPathReachable(TArray<FIntPoint> NewPath)
{
	PathReachable = NewPath;
}

void ACustomPlayerController::AddToPathReachable(FIntPoint NewPoint)
{
	PathReachable.AddUnique(NewPoint);
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

void ACustomPlayerController::SpawnBaseUnitGhost(EUnitType UnitToSpawn)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("SpawnBaseUnit"));
	if (Grid != nullptr)
	{
		FIntPoint SpawnLoc = FIntPoint(-999,-999);
		for (FIntPoint Index : BaseRef->AllSpawnLoc)
		{
			if (!Grid->GetGridData()->Find(Index)->TileState.Contains(EDC_TileState::Spawned))
			{
				SpawnLoc = Index;
				break;
			}
		}
		if(SpawnLoc != FIntPoint(-999,-999))
		{
			SpawnGhostUnit(UnitToSpawn, SpawnLoc);
		}
	}



	//Server_SpawnBaseUnit(UnitToSpawn, Grid, BaseRef, PlayerTeam);
}

void ACustomPlayerController::SpawnGhostUnit(EUnitType UnitToSpawn, FIntPoint SpawnChosen)
{
	AGhostUnitSpawning* GhostUnit = nullptr;
			if(PlayerStateRef)
			{
				
				switch (UnitToSpawn)
				{
				case EUnitType::U_Warrior:
					if(PlayerStateRef->GetStonePoints() >= PlayerStateRef->GetWarriorStoneCost() && PlayerStateRef->GetWoodPoints() >= PlayerStateRef->GetWarriorWoodCost() && PlayerStateRef->GetGoldPoints() >= PlayerStateRef->GetWarriorGoldCost())
					{
						PlayerStateRef->ChangeWoodPoints(PlayerStateRef->GetWarriorWoodCost(), false);
						PlayerStateRef->ChangeStonePoints(PlayerStateRef->GetWarriorStoneCost(), false);
						PlayerStateRef->ChangeGoldPoints(PlayerStateRef->GetWarriorGoldCost(), false);
						GhostUnit = GetWorld()->SpawnActor<AGhostUnitSpawning>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
						GhostUnit->SetUnitType(UnitToSpawn);
						GhostUnit->Spawn();
					}
					break;
				case EUnitType::U_Tank:
					if(PlayerStateRef->GetStonePoints() >= PlayerStateRef->GetTankStoneCost() && PlayerStateRef->GetWoodPoints() >= PlayerStateRef->GetTankWoodCost() && PlayerStateRef->GetGoldPoints() >= PlayerStateRef->GetTankGoldCost())
					{
						PlayerStateRef->ChangeWoodPoints(PlayerStateRef->GetTankWoodCost(), false);
						PlayerStateRef->ChangeStonePoints(PlayerStateRef->GetTankStoneCost(), false);
						PlayerStateRef->ChangeGoldPoints(PlayerStateRef->GetTankGoldCost(), false);
						GhostUnit = GetWorld()->SpawnActor<AGhostUnitSpawning>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
						GhostUnit->SetUnitType(UnitToSpawn);
						GhostUnit->Spawn();
					}
					break;
				case EUnitType::U_Mage:
					if(PlayerStateRef->GetStonePoints() >= PlayerStateRef->GetMageStoneCost() && PlayerStateRef->GetWoodPoints() >= PlayerStateRef->GetMageWoodCost() && PlayerStateRef->GetGoldPoints() >= PlayerStateRef->GetMageGoldCost())
					{
						PlayerStateRef->ChangeWoodPoints(PlayerStateRef->GetMageWoodCost(), false);
						PlayerStateRef->ChangeStonePoints(PlayerStateRef->GetMageStoneCost(), false);
						PlayerStateRef->ChangeGoldPoints(PlayerStateRef->GetMageGoldCost(), false);
						GhostUnit = GetWorld()->SpawnActor<AGhostUnitSpawning>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
						GhostUnit->SetUnitType(UnitToSpawn);
						GhostUnit->Spawn();
					}
					break;
				case EUnitType::U_Leader:
					if(PlayerStateRef->GetStonePoints() >= PlayerStateRef->GetLeaderStoneCost() && PlayerStateRef->GetWoodPoints() >= PlayerStateRef->GetLeaderWoodCost() && PlayerStateRef->GetGoldPoints() >= PlayerStateRef->GetLeaderGoldCost())
					{
						PlayerStateRef->ChangeWoodPoints(PlayerStateRef->GetLeaderWoodCost(), false);
						PlayerStateRef->ChangeStonePoints(PlayerStateRef->GetLeaderStoneCost(), false);
						PlayerStateRef->ChangeGoldPoints(PlayerStateRef->GetLeaderGoldCost(), false);
						GhostUnit = GetWorld()->SpawnActor<AGhostUnitSpawning>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
						GhostUnit->SetUnitType(UnitToSpawn);
						GhostUnit->Spawn();
					}
					break;
				default:
					break;
				}
			}
			UpdateUi();
			if(GhostUnit)
			{
				if(BaseRef)
					AllPlayerPassive.Add(FStructPassive( BaseRef,UnitToSpawn,SpawnChosen,GhostUnit));
				if(BuildingRef)
					AllPlayerPassive.Add(FStructPassive( BuildingRef,UnitToSpawn,SpawnChosen,GhostUnit));
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("AllPlayerPassive : " + FString::FromInt(AllPlayerPassive.Num())));
				Grid->GridVisual->addStateToTile(SpawnChosen, EDC_TileState::Spawned);
			}
			Grid->GridVisual->RemoveStateFromTile(SpawnChosen, EDC_TileState::Spawnable);
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

