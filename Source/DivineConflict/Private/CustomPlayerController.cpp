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

	// ----------------------------
	// Overrides

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

void ACustomPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACustomPlayerController::setGrid()
{
	//get the grid
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

	
	// ----------------------------
	// Replication (Variables)

void ACustomPlayerController::GetLifetimeReplicatedProps( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACustomPlayerController, PlayerTeam);
	DOREPLIFETIME(ACustomPlayerController, BaseRef);
	DOREPLIFETIME(ACustomPlayerController, BuildingRef);

}

	// ----------------------------
	// Replication (Functions)

void ACustomPlayerController::OnRep_PlayerTeam()
{
	AssignPlayerPosition();
}
	
	// ----------------------------
	// Interactions

//interract system of element on grid
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
			//get object for interract
		case EDC_ActionPlayer::None:
			if(Grid->GetGridData()->Find(PlayerPositionInGrid) != nullptr) 
			{
				if(PlayerStateRef->bIsActiveTurn)
				{
					//Active Turn
					//Interract tower
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
						
					//Interract unit
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
					////Interract building and have an unit on build
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
					//Interract building
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
					//Interract Base
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
			//Attack tower
			if(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile)
			{
				//check if the unit on tile is not on your team
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
								AllPlayerActions.Add(FStructActions(Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile, EDC_ActionPlayer::AttackBuilding, UnitRef));
							}
						}
					}
					// Are we attacking a Base?
					else if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->BaseOnTile)
					{
						if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->BaseOnTile->PlayerOwner != UnitRef->GetPlayerOwner())
						{
							AllPlayerActions.Add(FStructActions(Grid->GetGridData()->Find(PlayerPositionInGrid)->BaseOnTile, EDC_ActionPlayer::AttackBuilding,UnitRef));
						}
					}
					else
					{
						AllPlayerActions.Add(FStructActions(UnitRef, EDC_ActionPlayer::AttackUnit, nullptr));
					}
					//hide color on grid
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
			//Unit Move 
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
			//slect position for spawn unit
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
			//Special wizard
			if(UnitRef)
			{
				if(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile != nullptr)
				{
					if(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile->GetPlayerOwner() != UnitRef->GetPlayerOwner())
					{
						AllPlayerActions.Add(FStructActions(UnitRef, EDC_ActionPlayer::Special, Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile));
						UnitRef->HasActed = true;
					}
					if (Grid->GetGridData()->Find(PlayerPositionInGrid)->BaseOnTile)
					{
						AllPlayerActions.Add(FStructActions(Grid->GetGridData()->Find(PlayerPositionInGrid)->BaseOnTile, EDC_ActionPlayer::Special, UnitRef));
						UnitRef->HasActed = true;
					}
				}
				for(FIntPoint Index : PathReachable)
				{
					Grid->GridVisual->RemoveStateFromTile(Index, EDC_TileState::Attacked);
				}
				PathReachable.Empty();
				UnitRef->SetIsSelected(false);
				CameraPlayerRef->IsSpelling = false;
				PlayerAction = EDC_ActionPlayer::None;
				DisplayWidget();
			}
			break;
		default:
			break;
		}
	}
}
//check if player can interact with element on grid for widget 3D
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

	// ----------------------------
	// Mode Selection

void ACustomPlayerController::SelectModeMovement()
{
	FindReachableTiles();
	if (UnitRef)
	{
		
		if (Cast<AUnit_Child_Warrior>(UnitRef))
		{
			//si use a Special action
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

	//si unit has move
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
	//set color on grid
	for(FIntPoint Index : PathReachable)
    {
        Grid->GridVisual->addStateToTile(Index, EDC_TileState::Attacked);

    }
	PathReachable.Add(UnitRef->GetIndexPosition());
}

void ACustomPlayerController::SelectModeAttackBuilding()
{
	PlayerAction = EDC_ActionPlayer::AttackBuilding;
	PathReachable = Grid->GridPath->FindPath(TowerRef->GetGridPosition(),FIntPoint(-999,-999),true,3,false, false);
	for(FIntPoint Index : PathReachable)
	{
		Grid->GridVisual->addStateToTile(Index, EDC_TileState::Attacked);
	}
	CameraPlayerRef->IsTowering = true;
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
		//if unit has moved
			if (UnitRef->HasMoved)
			{
				PathReachable =  Grid->GridPath->FindPath(Grid->ConvertLocationToIndex(UnitRef->GetFinalGhostMesh()->GetComponentLocation()),
					FIntPoint(-999,-999),true,3,false,true);
				CameraPlayerRef->FullMoveDirection.X = UnitRef->GetFinalGhostMesh()->GetComponentLocation().X;
				CameraPlayerRef->FullMoveDirection.Y = UnitRef->GetFinalGhostMesh()->GetComponentLocation().Y;
				CameraPlayerRef->FullMoveDirection.Z = (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 175;
			} else
			{
				PathReachable =  Grid->GridPath->FindPath(UnitRef->GetIndexPosition(),FIntPoint(-999,-999),true,3,false,true);
			}
		//set color on grid
		for(FIntPoint Index : PathReachable)
		{
			Grid->GridVisual->addStateToTile(Index, EDC_TileState::Attacked);
		}
		PlayerAction = EDC_ActionPlayer::Special;
		break;
	default: ;
	}
}

void ACustomPlayerController::SelectModeSelectBuilding()
{
	PlayerAction = EDC_ActionPlayer::SelectBuilding;
}
	
	// ----------------------------
	// Turns - Set-up

//set player position on grid on start
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
						CameraPlayerRef->SetActorLocation(PlayerStart->GetActorLocation());
						CameraPlayerRef->FullMoveDirection = PlayerStart->GetActorLocation();
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

//switch playerTurn with server
void ACustomPlayerController::Server_SwitchPlayerTurn_Implementation(const ACustomGameState* GameState)
{
	Multi_SwitchPlayerTurn(GameState);
}

//call Switch Player Turn with multicast
void ACustomPlayerController::Multi_SwitchPlayerTurn_Implementation(const ACustomGameState* GameState)
{
	ACustomGameState* GameStateRef = const_cast<ACustomGameState*>(GameState);
	if(GameStateRef)
	{
		GameStateRef->SwitchPlayerTurn();
	}
}

//call delegate with server of passive leader
void ACustomPlayerController::Server_Delegate_Implementation()
{
	OnTurnChangedDelegate.Broadcast();
}
	
	// ----------------------------
	// End Turns

//call end turn with server
void ACustomPlayerController::EndTurn()
{
	Server_EndTurn();
	FeedbackEndTurn(true);
}

//call player end turn for GameState
void ACustomPlayerController::Server_EndTurn_Implementation()
{
	PlayerStateRef->SetIsReadyToSwitchTurn(true);
	Cast<ACustomGameState>(GetWorld()->GetGameState())->CheckSwitchPlayerTurn();
}

void ACustomPlayerController::FeedbackEndTurn_Implementation(bool visibility)
{
}
	
	// ----------------------------
	// Actions - Active

//attack base with server
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

//attack unit with server
void ACustomPlayerController::ServerAttackUnit_Implementation(AUnit* UnitToAttack, AUnit* UnitAttacking)
{
	if(UnitToAttack && UnitAttacking)
	{
		UnitAttacking->UnitToAttackRef = UnitToAttack;
		UnitToAttack->AnimAttack(UnitAttacking);
	}
}

//attack building with server
void ACustomPlayerController::ServerAttackBuilding_Implementation(ABuilding* BuildingToAttack, AUnit* UnitAttacking)
{
	if(BuildingToAttack && UnitAttacking)
		UnitAttacking->AttackBuilding(BuildingToAttack);
	else
	{
		Multi_ActionActiveTurn();
	}
}

//attack unit with tower with server
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

//use special unit with server
void ACustomPlayerController::Server_SpecialUnit_Implementation(AUnit* UnitSpecial,AActor* ThingToAttack)
{
	if(AUnit_Child_Mage* MageSp =  Cast<AUnit_Child_Mage>(UnitSpecial))
		MageSp->SpecialMage(ThingToAttack);
	else UnitSpecial->Special();
}

	
	// ----------------------------
	// Actions - Passive

//get tile valid for spawn
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

//call spawn unit with server
bool ACustomPlayerController::SpawnUnit(EUnitType UnitToSpawn, FIntPoint SpawnChosen,ABase* BaseToSpawn, ABuilding* BuildingToSpawn)
{
	Server_SpawnUnit(UnitToSpawn, SpawnChosen, BaseToSpawn, BuildingToSpawn,PlayerStateRef);
	return Grid->GetGridData()->Find(SpawnChosen)->UnitOnTile != nullptr;
}

//spawn unit with server
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
}

//set unit on grid and team with multicast
void ACustomPlayerController::Multicast_SpawnUnit_Implementation(AUnit* UnitSpawned,AGrid* GridSpawned, ACustomPlayerState* PlayerStatRef,  ABase* BaseSpawned, ABuilding* BuildingSpawned)
{
	if(UnitSpawned)
	{
		UnitSpawned->Grid = Grid;		
		if(BuildingSpawned)
		{
			UnitSpawned->SetPlayerOwner(BuildingSpawned->PlayerOwner);
		}
			
		if(BaseSpawned)
		{
			UnitSpawned->SetPlayerOwner(BaseSpawned->PlayerOwner);
		}
	}
	Client_UpdateUi();
}

//get plsition for spawn unit with base
void ACustomPlayerController::Server_SpawnBaseUnit_Implementation(EUnitType UnitToSpawn,AGrid* GridSer, ABase* BaseToSpawn, EPlayer PlayerOwner)
{
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

//get position for spawn unit ghost for base
void ACustomPlayerController::SpawnBaseUnitGhost(EUnitType UnitToSpawn)
{
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

//Spawn ghost on client
void ACustomPlayerController::SpawnGhostUnit(EUnitType UnitToSpawn, FIntPoint SpawnChosen)
{
	AGhostUnitSpawning* GhostUnit = nullptr;
			if(PlayerStateRef)
			{
				
				switch (UnitToSpawn)
				{
				case EUnitType::U_Warrior:
					//if you have enough resources
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
				Grid->GridVisual->addStateToTile(SpawnChosen, EDC_TileState::Spawned);
			}
			Grid->GridVisual->RemoveStateFromTile(SpawnChosen, EDC_TileState::Spawnable);
}
	
	// ----------------------------
	// Cancel Actions

//cancel action (on final version)
void ACustomPlayerController::CancelLastAction()
{
	if (!AllPlayerActions.IsEmpty() && PlayerStateRef->bIsActiveTurn)
	{
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

	// ----------------------------
	// Actions - End Turn

//call Action Active Turn with server
void ACustomPlayerController::Server_ActionActiveTurn_Implementation()
{
	Multi_ActionActiveTurn();
}

//execute action active turn with multicast
void ACustomPlayerController::Multi_ActionActiveTurn_Implementation()
{
	if(IsLocalController())
	{
		HiddeWidget();
		//if PlayerState si set
		if(PlayerStateRef == nullptr)
			PlayerStateRef = Cast<ACustomPlayerState>(PlayerState);
		GetWorld()->GetTimerManager().ClearTimer(TimerActiveEndTurn);
		
		//if player is active
		if(PlayerStateRef->bIsActiveTurn)
		{
			//if there is action to do
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
				//switch on action
				switch (AllPlayerActions[0].UnitAction)
				{
				case EDC_ActionPlayer::MoveUnit:
					//move unit
					ServerMoveUnit(UnitAction);
					AllPlayerActions.RemoveAt(0);
					break;
				case EDC_ActionPlayer::AttackUnit:
					//attack unit
					ServerAttackUnit(UnitAction, AllPlayerActions[0].UnitAttacking);
					GetWorld()->GetTimerManager().SetTimer(TimerActiveEndTurn, this, &ACustomPlayerController::Multi_ActionActiveTurn, 0.5f, false);
					AllPlayerActions.RemoveAt(0);
					break;
				case EDC_ActionPlayer::AttackBuilding:
					//attack tower
					if(TowerAction)
					{
						ServerAttackTower(TowerAction, AllPlayerActions[0].UnitAttacking);
						GetWorld()->GetTimerManager().SetTimer(TimerActiveEndTurn, this, &ACustomPlayerController::Multi_ActionActiveTurn, 0.5f, false);
						TowerAction = nullptr;
					}
					//attack base with unit
					if(BaseAction)
					{
						AttackBase(BaseAction, AllPlayerActions[0].UnitAttacking);
						GetWorld()->GetTimerManager().SetTimer(TimerActiveEndTurn, this, &ACustomPlayerController::Multi_ActionActiveTurn, 0.5f, false);
					}
					//attack building with unit
					if (BuildingAction)
					{
						ServerAttackBuilding(BuildingAction, AllPlayerActions[0].UnitAttacking);
						GetWorld()->GetTimerManager().SetTimer(TimerActiveEndTurn, this, &ACustomPlayerController::Multi_ActionActiveTurn, 0.5f, false);
						BuildingAction = nullptr;
					}
					AllPlayerActions.RemoveAt(0);
					break;
				case EDC_ActionPlayer::Special:
					//if special action on base
					if(BaseAction)
						Server_SpecialUnit(AllPlayerActions[0].UnitAttacking, BaseAction);
					//if special action
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
				//if no action left
				AllPlayerActions.Empty();
				GetWorld()->GetTimerManager().ClearTimer(TimerActiveEndTurn);
				//call check player action passive with server
				Server_CheckPlayerActionPassive();
			}
		}
	}
}

//call check player action passive on Game State with server
void ACustomPlayerController::Server_CheckPlayerActionPassive_Implementation()
{
	Cast<ACustomGameState>(GetWorld()->GetGameState())->CheckPlayerActionPassive();
}

//call Passive Active Turn with server
void ACustomPlayerController::Server_ActionPassiveTurn_Implementation()
{
	Multi_ActionPassiveTurn();
}

//execute action passive turn with multicast
void ACustomPlayerController::Multi_ActionPassiveTurn_Implementation()
{
	if(IsLocalController())
	{
		//if there is action to do
		if(!AllPlayerPassive.IsEmpty())
		{
			//if Building Action
			if(ABuilding *BuildingAction = Cast<ABuilding>(AllPlayerPassive[0].ActorRef))
			{
				GetWorld()->DestroyActor(AllPlayerPassive[0].GhostRef);
				Grid->GridVisual->RemoveStateFromTile(AllPlayerPassive[0].TilePosition, EDC_TileState::Spawned);
				Server_SpawnUnit(BuildingAction->UnitProduced, AllPlayerPassive[0].TilePosition, nullptr, BuildingAction, PlayerStateRef);
				AllPlayerPassive.RemoveAt(0);
				Multi_ActionPassiveTurn();
			}
			//if Base Action
			else if(ABase *BaseAction = Cast<ABase>(AllPlayerPassive[0].ActorRef))
			{
				GetWorld()->DestroyActor(AllPlayerPassive[0].GhostRef);
				Grid->GridVisual->RemoveStateFromTile(AllPlayerPassive[0].TilePosition, EDC_TileState::Spawned);
				Server_SpawnBaseUnit(AllPlayerPassive[0].UnitType,Grid,BaseAction,PlayerStateRef->PlayerTeam);
				AllPlayerPassive.RemoveAt(0);
				Multi_ActionPassiveTurn();
			}
		}
		else
		{
			//if no action left
			Server_SwitchPlayerTurn(Cast<ACustomGameState>(GetWorld()->GetGameState()));
			Server_Delegate();
		}
	}

}
	
// ----------------------------
// "Pathfinding"

void ACustomPlayerController::ServerMoveUnit_Implementation(const AUnit* UnitToMove)
{
	AUnit* RefUnit = const_cast<AUnit*>(UnitToMove);
	
	if(UnitToMove)
	{
		RefUnit->MoveUnitEndTurn();
		UnitRef = nullptr;
	}
}

void ACustomPlayerController::Server_PrepareMoveUnit_Implementation(const TArray<FIntPoint>& Path,
                                                                    const AUnit* UnitToMove)
{
	AUnit *UnitRefServer = const_cast<AUnit*>(UnitToMove);
		
	UnitRefServer->Multi_PrepareMove(Path);
}

//get all tile for reach movement
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
	
	// ----------------------------
	// UI

		// General

void ACustomPlayerController::UpdateUi_Implementation()
{
}

void ACustomPlayerController::Client_UpdateUi_Implementation()
{
	UpdateUi();
}

void ACustomPlayerController::UpdateWidget3D_Implementation(int Index, bool bVisibility)
{
}

		// Stats

void ACustomPlayerController::DisplayWidget_Implementation()
{
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

		// Attack

// Display Widget 3D on camera depending if item is interactive or not
void ACustomPlayerController::DisplayAttackWidget_Implementation()
{
}

void ACustomPlayerController::RemoveAttackWidget_Implementation()
{
}

		// Timers

void ACustomPlayerController::UpdateUITimer_Implementation(int TimeLeft)
{
}

void ACustomPlayerController::Client_PauseTimerUI_Implementation()
{
	PauseTimerUI();
}

void ACustomPlayerController::PauseTimerUI_Implementation()
{
}

		// Hide

void ACustomPlayerController::HiddeWidget_Implementation()
{
}

	// ----------------------------
	// GETTERS

		// Turns

bool ACustomPlayerController::GetIsInActiveTurn()
{
	return IsInActiveTurn;
}

bool ACustomPlayerController::GetIsReady()
{
	return IsReady;
}

		// Player Team

EPlayer ACustomPlayerController::GetPlayerTeam()
{
	return PlayerTeam;
}

		// Player Actions

//get player action
EDC_ActionPlayer ACustomPlayerController::GetPlayerAction()
{
	return PlayerAction;
}

		// "PathFinding"

TArray<FIntPoint> ACustomPlayerController::GetPathReachable()
{
	return PathReachable;
}
	
	// ----------------------------
	// SETTERS

		//Turns

void ACustomPlayerController::SetIsInActiveTurn(bool bA)
{
	IsInActiveTurn = bA;
}

void ACustomPlayerController::SetIsReady(bool bR)
{
	IsReady = bR;
}

		// Player Team

void ACustomPlayerController::SetPlayerTeam(EPlayer PT)
{
	PlayerTeam = PT;
}

		// Player Actions

void ACustomPlayerController::SetPlayerAction(EDC_ActionPlayer PA)
{
	PlayerAction = PA;
}

		// "PathFinding"

void ACustomPlayerController::AddToPathReachable(FIntPoint NewPoint)
{
	PathReachable.AddUnique(NewPoint);
}

void ACustomPlayerController::SetPathReachable(TArray<FIntPoint> NewPath)
{
	PathReachable = NewPath;
}




	
	// ----------------------------
	// TO DELETE!!!!!!!!!!!!!!!!!!!

/*void ACustomPlayerController::DisplayWidgetEndGame_Implementation()
{
}*/

bool ACustomPlayerController::GetIsHell()
{
	return IsHell;
}

void ACustomPlayerController::SetIsHell(bool bH)
{
	IsHell = bH;
}