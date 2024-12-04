// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPlayerController.h"

#include "Base.h"
#include "Building.h"
#include "BuildingSpawnLocation.h"
#include "EnhancedInputSubsystems.h"
#include "CustomGameState.h"
#include "Engine/LocalPlayer.h"
#include "CameraPlayer.h"
#include "EnhancedInputComponent.h"
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

	CameraPlayerRef = Cast<ACameraPlayer>(GetPawn());
	
	SetupInputComponent();
	
	UpdateWidget3D(false, false);
	
	if(CameraPlayerRef != nullptr)
	{
		CameraPlayerRef->SetCustomPlayerController(this);
	}
	SetGrid();
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACustomPlayerController::AssignPlayerPosition, 2.f, false);
	GetWorld()->GetTimerManager().SetTimer(InactivityTimerHandle, this, &ACustomPlayerController::IncrementInactivityTimer, 1.0f, true);
}

void ACustomPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (Cast<APlayerController>(this))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	if(CameraPlayerRef)
		if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			EnhancedInputComponent->BindAction(AIEndTurn, ETriggerEvent::Started, this, &ACustomPlayerController::EndTurn);
			EnhancedInputComponent->BindAction(AIMove,ETriggerEvent::Triggered, CameraPlayerRef, "UpdatedMove");
			EnhancedInputComponent->BindAction(AIMove, ETriggerEvent::Started, CameraPlayerRef, "RepeatMoveTimerCamera");
			EnhancedInputComponent->BindAction(AIMove, ETriggerEvent::Completed, CameraPlayerRef, "StopRepeatMoveTimerCamera");
			EnhancedInputComponent->BindAction(AIRotate, ETriggerEvent::Started, this, &ACustomPlayerController::RotateCamera);
			EnhancedInputComponent->BindAction(AIRotate, ETriggerEvent::Triggered, this, &ACustomPlayerController::RotateCameraPitch );
			EnhancedInputComponent->BindAction(AIZoom, ETriggerEvent::Triggered, this, &ACustomPlayerController::ZoomCamera);
			EnhancedInputComponent->BindAction(AIInteraction,ETriggerEvent::Started, this, &ACustomPlayerController::ControllerInteraction);
			EnhancedInputComponent->BindAction(AICancel,ETriggerEvent::Started, this, &ACustomPlayerController::CancelAction);
		}
}

void ACustomPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACustomPlayerController::SetGrid()
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
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACustomPlayerController::SetGrid, 0.2f, false);
		
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
	DOREPLIFETIME(ACustomPlayerController, CameraPlayerRef);

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
		if (PlayerStateRef)
			if (PlayerStateRef->GetActionPoints() < 1)
				return;
		
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
					bool bIsTower = false;
					bool bIsBuilding = false;
					bool bIsUnit = false;
					if (Grid->GetGridData()->Find(PlayerPositionInGrid)->TowerOnTile)
						bIsTower = true;
					if (Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile)
						if (Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile->GetIsGarrison())
							bIsBuilding = true;
						else
							bIsUnit = true;
					if (Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile)
						bIsBuilding = true;
							
					
					//Active Turn
					//Interract tower
					if(bIsTower)
					{
						if(Grid->GetGridData()->Find(PlayerPositionInGrid)->TowerOnTile->GetPlayerOwner() == PlayerStateRef->PlayerTeam)
						{
							TowerRef = Grid->GetGridData()->Find(PlayerPositionInGrid)->TowerOnTile;
							TowerRef->bIsSelected = true;
							TowerRef->PlayerController = this;
							DisplayWidgetTower();
						}
					}
						
					//Interract unit
					else if (bIsUnit)
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
					else if(bIsBuilding)
					{
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Building"));
						bool bCanInteract = false;
						// if (Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile)
						// 	if(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile->GetPlayerOwner() == PlayerStateRef->PlayerTeam)
						// 	{
						// 		bCanInteract = true;
						// 		UnitRef = Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile;
						// 		BuildingRef = Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile->GetBuildingRef();
						// 	}
						if (Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile->UnitRef)
							if (Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile->UnitRef->GetPlayerOwner() == PlayerStateRef->PlayerTeam)
							{
								bCanInteract = true;
								UnitRef = Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile->UnitRef;
								BuildingRef = Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile;
							}
						if(bCanInteract)
						{
                                CameraPlayerRef->SetCustomPlayerController(this);
                                CameraPlayerRef->UnitMovingCurrentMovNumber = UnitRef->GetPM();
                                //IInteractInterface::Execute_Interact(Grid->GridData.Find(PlayerPositionInGrid)->BuildingOnTile->UnitRef, this);
                                IInteractInterface::Execute_Interact(Grid->GridData.Find(PlayerPositionInGrid)->BuildingOnTile, this);
                                DisplayWidgetBuilding();
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
								Grid->GridVisual->AddStateToTile(BuildingIndex, EDC_TileState::Selected);
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
			RemoveAttackWidget();
			break;
		case EDC_ActionPlayer::AttackBuilding:
			//Attack tower
			if(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile)
			{
				//check if the unit on tile is not on your team
				if(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile->GetPlayerOwner() != PlayerStateRef->PlayerTeam)
				{
					TowerRef->PrepareAttack(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile);
					AllPlayerActions.Add(FStructActiveActions(TowerRef, EDC_ActionType::TowerAttack, EDC_TargetType::Unit, PlayerPositionInGrid));
					PlayerAction = EDC_ActionPlayer::None;
					PlayerStateRef->SetActionPoints(PlayerStateRef->GetActionPoints() - 1);
					TowerRef->SetCanAttack(false);
				}
				CameraPlayerRef->IsTowering = false;
			}
			RemoveAttackWidget();
			break;
		case EDC_ActionPlayer::AttackUnit:
			if (UnitRef)
			{
				if (!UnitRef->bHasActed && UnitRef->GetIndexPosition() != PlayerPositionInGrid){
					// Are we attacking a Unit?
					if(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile)
					{
						if (Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile->GetPlayerOwner() != PlayerStateRef->PlayerTeam)
						{
							AllPlayerActions.Add(FStructActiveActions(UnitRef, EDC_ActionType::UnitAttack, EDC_TargetType::Unit, PlayerPositionInGrid));
						}
					}
					// Are we attacking a Building?
					else if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->BuildingOnTile)
					{
						if(Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->BuildingOnTile->PlayerOwner != PlayerStateRef->PlayerTeam)
						{
							if(Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->BuildingOnTile->GarrisonFull)
							{
								AllPlayerActions.Add(FStructActiveActions(UnitRef, EDC_ActionType::UnitAttack, EDC_TargetType::Building, PlayerPositionInGrid));
							}
						}
					}
					// Are we attacking a Base?
					else if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->BaseOnTile)
					{
						if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation()))->BaseOnTile->PlayerOwner != UnitRef->GetPlayerOwner())
						{
							AllPlayerActions.Add(FStructActiveActions(UnitRef, EDC_ActionType::UnitAttack, EDC_TargetType::Base, PlayerPositionInGrid));
						}
					}
					else
					{
						AllPlayerActions.Add(FStructActiveActions(UnitRef, EDC_ActionType::UnitAttack, EDC_TargetType::None, PlayerPositionInGrid));
					}
					//hide color on grid
					for(FIntPoint Index : PathReachable)
					{
						Grid->GridVisual->RemoveStateFromTile(Index, EDC_TileState::Attacked);
					}
					PathReachable.Empty();
					UnitRef->SetIsSelected(false);
					UnitRef->bHasActed = true;
					PlayerStateRef->SetActionPoints(PlayerStateRef->GetActionPoints() - 1);
					CameraPlayerRef->IsAttacking = false;
					PlayerAction = EDC_ActionPlayer::None;
		
					RemoveAttackWidget();
					if (!UnitRef->bHasActed || !UnitRef)
						DisplayWidget();
				}
				Grid->GridVisual->RemoveStateFromTile(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection), EDC_TileState::Hovered);
				if (!UnitRef->bHasMoved)
				{
					CameraPlayerRef->FullMoveDirection.X = UnitRef->GetActorLocation().X;
					CameraPlayerRef->FullMoveDirection.Y = UnitRef->GetActorLocation().Y;
					CameraPlayerRef->FullMoveDirection.Z = (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 175;
				} else
				{
					CameraPlayerRef->FullMoveDirection.X = UnitRef->GetFinalGhostMesh()->GetComponentLocation().X;
					CameraPlayerRef->FullMoveDirection.Y = UnitRef->GetFinalGhostMesh()->GetComponentLocation().Y;
					CameraPlayerRef->FullMoveDirection.Z = (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 175;
				}
				Grid->GridVisual->AddStateToTile(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection), EDC_TileState::Hovered);
				RemoveAttackWidget();
			}
			break;
		case EDC_ActionPlayer::MoveUnit:
			//Unit Move 
			if (UnitRef)
			{
				if (!UnitRef->bHasMoved)
				{
					for(FIntPoint Index : PathReachable)
					{
						Grid->GridVisual->RemoveStateFromTile(Index, EDC_TileState::Reachable);
					}
					for(FIntPoint Index : CameraPlayerRef->Path)
					{
						Grid->GridVisual->RemoveStateFromTile(Index, EDC_TileState::Pathfinding);
					}
					
					if (UnitRef->bHasActed)
						UnitRef->bFirstActionIsMove = false;
					else
						UnitRef->bFirstActionIsMove = true;
					

					if (CameraPlayerRef->Path.Num() > 1)
					{
						if (UnitRef->GetIsGarrison())
						{
							CameraPlayerRef->Path.RemoveAt(0);
						}
						UnitRef->SetIsSelected(false);
						Server_PrepareMoveUnit(CameraPlayerRef->Path,UnitRef);
						UnitRef->InitGhosts_Implementation();
						//if (!UnitRef->HasActed)
						//	UnitRef->InitGhosts_Implementation();
						
						AllPlayerActions.Add(FStructActiveActions(UnitRef, EDC_ActionType::UnitMove, EDC_TargetType::None, PlayerPositionInGrid));
						UnitRef->bHasMoved = true;
						PlayerStateRef->SetActionPoints(PlayerStateRef->GetActionPoints() - 1);
					}
					PathReachable.Empty();
					CameraPlayerRef->IsMovingUnit = false;
					CameraPlayerRef->Path.Empty();
					
					PlayerAction = EDC_ActionPlayer::None;
					if (!UnitRef->bHasActed || !UnitRef)
					DisplayWidget();
				}
			}
			RemoveAttackWidget();
			break;
		case EDC_ActionPlayer::SelectBuilding:
			//select position for spawn unit
			AllPossibleSpawns = PrepareSpawnArea(BuildingRef->AllSpawnLoc, BuildingRef->SpawnLocRef[0]);
			BuildingRef->BuildingSpawnLocationRef->DespawnGridColors(BuildingRef->AllSpawnLoc);
			AllCurrentSpawnPoints.Empty();
			CameraPlayerRef->IsSpawningUnit = false;
			CameraPlayerRef->FirstMove = true;
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
			RemoveAttackWidget();
			break;
		case EDC_ActionPlayer::Special:
			//Special wizard

				/* DEPRECIATED
			if(UnitRef)
			{
				if(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile != nullptr)
				{
					if(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile->GetPlayerOwner() != UnitRef->GetPlayerOwner())
					{
						
						AllPlayerActions.Add(FStructActions(UnitRef, EDC_ActionPlayer::Special, Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile));
						UnitRef->HasActed = true;
						PlayerStateRef->SetActionPoints(PlayerStateRef->GetActionPoints() - 2);
					}
				}
				else if (Grid->GetGridData()->Find(PlayerPositionInGrid)->BaseOnTile)
				{
					AllPlayerActions.Add(FStructActions(Grid->GetGridData()->Find(PlayerPositionInGrid)->BaseOnTile, EDC_ActionPlayer::Special, UnitRef));
					UnitRef->HasActed = true;
					PlayerStateRef->SetActionPoints(PlayerStateRef->GetActionPoints() - 2);
				}
				else if (Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile->GarrisonFull)
				{
					AllPlayerActions.Add(FStructActions(UnitRef, EDC_ActionPlayer::Special, Grid->GetGridData()->Find(PlayerPositionInGrid)->BuildingOnTile->UnitRef));
					UnitRef->HasActed = true;
					PlayerStateRef->SetActionPoints(PlayerStateRef->GetActionPoints() - 2);
				}
				
				for(FIntPoint Index : PathReachable)
				{
					Grid->GridVisual->RemoveStateFromTile(Index, EDC_TileState::Attacked);
				}
				PathReachable.Empty();
				UnitRef->SetIsSelected(false);
				CameraPlayerRef->IsSpelling = false;
				PlayerAction = EDC_ActionPlayer::None;
				if (!UnitRef->HasActed || !UnitRef)
				DisplayWidget();
			}
			RemoveAttackWidget();
			*/
			break;
		default:
			break;
		}
	}
	UpdateUi();
	RemoveAttackWidget();
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
		bIsUIVisible = false;
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
		
		else if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->BuildingOnTile)
		{
			if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->BuildingOnTile->PlayerOwner == PlayerStateRef->PlayerTeam && PlayerStateRef->bIsActiveTurn)
			{			
				UpdateWidget3D(0, true);
			}
			else UpdateWidget3D(1, true);
		}

		else UpdateWidget3D(1, false);
	}
	
	else if (PlayerAction == EDC_ActionPlayer::AttackUnit)
	{
		if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->UnitOnTile != nullptr
			&& !bIsUIVisible)
		{
			if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->UnitOnTile->GetPlayerOwner() != PlayerStateRef->PlayerTeam)
			{
				Opponent = Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->UnitOnTile;
				DisplayAttackWidget();
				bIsUIVisible = true;
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
		else
		{
			RemoveAttackWidget();
			bIsUIVisible = false;
		}
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
	bHoveringOverUnit = false;
	DisplayWidgetHovering();
	
	FindReachableTiles();
	if (UnitRef)
	{
		CameraPlayerRef->FirstMove = true;
		if (Cast<AUnit_Child_Warrior>(UnitRef))
		{
			// if use a Special action
			if (UnitRef->bHasActed &&  UnitRef->GetFinalGhostMesh()->IsVisible())
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
	bHoveringOverUnit = false;
	DisplayWidgetHovering();
	
	PlayerAction = EDC_ActionPlayer::AttackUnit;
	CameraPlayerRef->IsAttacking = true;

	//si unit has moved
	if (UnitRef->bHasMoved)
	{
		PathReachable = Grid->GridPath->FindPath(Grid->ConvertLocationToIndex(UnitRef->GetFinalGhostMesh()->GetComponentLocation()),FIntPoint(-999,-999),true,2,false, true);
		Grid->GridVisual->RemoveStateFromTile(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection), EDC_TileState::Hovered);
		CameraPlayerRef->FullMoveDirection.X = UnitRef->GetFinalGhostMesh()->GetComponentLocation().X;
		CameraPlayerRef->FullMoveDirection.Y = UnitRef->GetFinalGhostMesh()->GetComponentLocation().Y;
		CameraPlayerRef->FullMoveDirection.Z = (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 175;
	} else
	{
		PathReachable = Grid->GridPath->FindPath(UnitRef->GetIndexPosition(),FIntPoint(-999,-999),true,2,false, true);
	}
	//set color on grid
	for(FIntPoint Index : PathReachable)
    {
        Grid->GridVisual->AddStateToTile(Index, EDC_TileState::Attacked);
    }
	PathReachable.Add(UnitRef->GetIndexPosition());
}

void ACustomPlayerController::SelectModeAttackBuilding()
{
	bHoveringOverUnit = false;
	DisplayWidgetHovering();
	
	PlayerAction = EDC_ActionPlayer::AttackBuilding;
	PathReachable = Grid->GridPath->FindPath(TowerRef->GetGridPosition(),FIntPoint(-999,-999),true,3,false, false);
	for(FIntPoint Index : PathReachable)
	{
		Grid->GridVisual->AddStateToTile(Index, EDC_TileState::Attacked);
	}
	CameraPlayerRef->IsTowering = true;
}

void ACustomPlayerController::SelectModeSpecial()
{
	/* DEPRECIATED
	bHoveringOverUnit = false;
	DisplayWidgetHovering();
	
	if (PlayerStateRef)
		if (PlayerStateRef->GetActionPoints() < 2)
			return;
	
	switch (UnitRef->UnitName)
	{
	case EUnitName::Warrior:
		//Warrior
			if (UnitRef->HasMoved)
			{
				if (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(UnitRef->GetFinalGhostMesh()->GetComponentLocation()))->UpwallOnTile)
				{
					AllPlayerActions.Add(FStructActions(UnitRef, EDC_ActionPlayer::Special));
					UnitRef->Special();
					//AllPlayerActions.Add(FStructActions(UnitRef, EDC_ActionPlayer::MoveUnit));
					UnitRef->HasActed = true;		
					PlayerStateRef->SetActionPoints(PlayerStateRef->GetActionPoints() - 2);
				}
			}
			else
			{
				if (Grid->GetGridData()->Find(UnitRef->GetIndexPosition())->UpwallOnTile)
				{
					AllPlayerActions.Add(FStructActions(UnitRef, EDC_ActionPlayer::Special));
					UnitRef->Special();
					//AllPlayerActions.Add(FStructActions(UnitRef, EDC_ActionPlayer::MoveUnit));
					UnitRef->HasActed = true;		
					PlayerStateRef->SetActionPoints(PlayerStateRef->GetActionPoints() - 2);
				}
			}
		break;
	case EUnitName::Tank:
		//Tank
		AllPlayerActions.Add(FStructActiveActions(UnitRef, EDC_ActionPlayer::Special));
		UnitRef->HasActed = true;
		PlayerStateRef->SetActionPoints(PlayerStateRef->GetActionPoints() - 2);
		break;
	case EUnitName::Mage:
		//Mage
			if (CameraPlayerRef)
				CameraPlayerRef->IsSpelling = true;
		//if unit has moved
			if (UnitRef->HasMoved)
			{
				PathReachable =  Grid->GridPath->NewFindPathSpMage(Grid->ConvertLocationToIndex(UnitRef->GetFinalGhostMesh()->GetComponentLocation()),this);
				Grid->GridVisual->RemoveStateFromTile(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection), EDC_TileState::Hovered);
				CameraPlayerRef->FullMoveDirection.X = UnitRef->GetFinalGhostMesh()->GetComponentLocation().X;
				CameraPlayerRef->FullMoveDirection.Y = UnitRef->GetFinalGhostMesh()->GetComponentLocation().Y;
				CameraPlayerRef->FullMoveDirection.Z = (Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 175;
			} else
			{
				PathReachable =  Grid->GridPath->NewFindPathSpMage(UnitRef->GetIndexPosition(),this);
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
	*/
}


void ACustomPlayerController::SelectModeSelectBuilding()
{
	bHoveringOverUnit = false;
	DisplayWidgetHovering();
	
	PlayerAction = EDC_ActionPlayer::SelectBuilding;
}

void ACustomPlayerController::UpgradeBase(ABase* BaseToUpgrade)
{
	if (BaseToUpgrade)
	{
		if(PlayerStateRef){
			// Check if player has enough resources
			if (PlayerStateRef->GetWoodPoints() >= BaseToUpgrade->GetWoodCostUpgrade() && PlayerStateRef->GetStonePoints() >= BaseToUpgrade->GetStoneCostUpgrade() && PlayerStateRef->GetGoldPoints() >= BaseToUpgrade->GetGoldCostUpgrade())
			{
				if (BaseToUpgrade->Level < BaseToUpgrade->MaxLevel)
				{
					if (!HasAuthority())
						PlayerStateRef->SetMaxUnits(PlayerStateRef->GetMaxUnits() + 5);
					Server_UpgradeBase();
					PlayerStateRef->ChangeWoodPoints(BaseToUpgrade->GetWoodCostUpgrade(), false);
					PlayerStateRef->ChangeStonePoints(BaseToUpgrade->GetStoneCostUpgrade(), false);
					PlayerStateRef->ChangeGoldPoints(BaseToUpgrade->GetGoldCostUpgrade(), false);
					BaseToUpgrade->SetCostsUpgrade(BaseToUpgrade->GetGoldCostUpgrade() + 10, BaseToUpgrade->GetStoneCostUpgrade() + 10, BaseToUpgrade->GetWoodCostUpgrade() + 10);
					BaseToUpgrade->Level++;
				}
			}
		}
	}
}

void ACustomPlayerController::Server_UpgradeBase_Implementation()
{
	PlayerStateRef->SetMaxUnits(PlayerStateRef->GetMaxUnits() + 5);
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
	
void ACustomPlayerController::SetPositionInBase()
{	
	TArray<AActor*> ActorFounds;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABase::StaticClass(), ActorFounds);
	for(AActor* CurrentActor : ActorFounds)
	{
		if(ABase* CurrentBase = Cast<ABase>(CurrentActor))
		{
			if (PlayerStateRef)
			{
				if(CurrentBase->PlayerOwner == PlayerStateRef->PlayerTeam)
				{
					if (CameraPlayerRef)
						Multi_SetPositionInBase(CurrentBase);
					else
					{
						CameraPlayerRef = Cast<ACameraPlayer>(GetPawn());
						SetPositionInBase();
					}
				}
			}
		}
	}
}

void ACustomPlayerController::Multi_SetPositionInBase_Implementation(ABase* CurrentBase)
{
	Grid->GridVisual->RemoveStateFromTile(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection), EDC_TileState::Hovered);
	CameraPlayerRef->FullMoveDirection = CurrentBase->GetActorLocation();
}

void ACustomPlayerController::SetPositionOnUnit()
{
	TArray<AActor*> ActorFounds;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnit::StaticClass(), ActorFounds);
	for(AActor* CurrentActor : ActorFounds)
	{
		if(AUnit* CurrentUnit = Cast<AUnit>(CurrentActor))
		{
			if (PlayerStateRef)
			{
				if(CurrentUnit->GetPlayerOwner() == PlayerStateRef->PlayerTeam)
				{
					if (CameraPlayerRef)
					{
						Multi_SetPositionOnUnit(CurrentUnit);
						return;
					}
					else
					{
						CameraPlayerRef = Cast<ACameraPlayer>(GetPawn());
						SetPositionOnUnit();
					}
				}
			}
		}
	}
}

void ACustomPlayerController::Multi_SetPositionOnUnit_Implementation(AUnit* CurrentUnit)
{
	Grid->GridVisual->RemoveStateFromTile(Grid->ConvertLocationToIndex(CameraPlayerRef->FullMoveDirection), EDC_TileState::Hovered);
	CameraPlayerRef->FullMoveDirection = CurrentUnit->GetActorLocation();
}


// ----------------------------
	// End Turns

//call end turn with server
void ACustomPlayerController::EndTurn()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("EndTurn"));
	Server_EndTurn();
	FeedbackEndTurn(true);
}

void ACustomPlayerController::Server_ClearMode_Implementation()
{
	Multi_ClearMode();
}

void ACustomPlayerController::Multi_ClearMode_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ClearMode"));
	if(CameraPlayerRef)
		CameraPlayerRef->ClearMoveMode();
	for(FIntPoint Index : PathReachable)
	{
		Grid->GridVisual->RemoveStateFromTile(Index, EDC_TileState::Reachable);
	}
	PathReachable.Empty();
	PlayerAction = EDC_ActionPlayer::None;
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
}

//attack unit with tower with server
void ACustomPlayerController::ServerAttackTower_Implementation(ATower* TowerToAttack, AUnit* UnitAttacking)
{
	if(TowerToAttack && UnitAttacking)
	{
		TowerToAttack->AttackUnit(UnitAttacking,this);
	}
}

//use special unit with server
void ACustomPlayerController::Server_SpecialUnit_Implementation(AUnit* UnitSpecial,AActor* ThingToAttack)
{
/* DEPRECIATED
	if(AUnit_Child_Mage* MageSp =  Cast<AUnit_Child_Mage>(UnitSpecial))
	{
		MageSp->SpecialMage(ThingToAttack);
		GetWorld()->GetTimerManager().SetTimer(TimerActiveEndTurn, this, &ACustomPlayerController::Multi_ActionActiveTurn, 1.5f, false);
	}
	else if(AUnit_Child_Warrior* WarriorSp =  Cast<AUnit_Child_Warrior>(UnitSpecial))
	{
		WarriorSp->Server_MoveToClimb();
		//if (!WarriorSp->FirstActionIsMove)
			//GetWorld()->GetTimerManager().SetTimer(TimerActiveEndTurn, this, &ACustomPlayerController::Multi_ActionActiveTurn, 0.5f, false);
	}
	else
	{
		UnitSpecial->Special();
		GetWorld()->GetTimerManager().SetTimer(TimerActiveEndTurn, this, &ACustomPlayerController::Multi_ActionActiveTurn, 0.5f, false);
	}
	*/
}
	
	// ----------------------------
	// Actions - Passive

//get tile valid for spawn
TArray<FIntPoint> ACustomPlayerController::PrepareSpawnArea(TArray<FIntPoint> AllSpawnArea, FIntPoint BaseTile)
{
	TArray<FIntPoint> TrueSpawnArea;
	for (FIntPoint Index : AllSpawnArea)
	{
		if (Grid->GridPath->IsValidHeight(Grid->GetGridData()->Find(Index), Grid->GetGridData()->Find(BaseTile)))
		{
			TrueSpawnArea.Add(Index);
		}
	}
	return TrueSpawnArea;
}

//call spawn unit with server
bool ACustomPlayerController::SpawnUnit(EUnitType UnitToSpawn, FIntPoint SpawnChosen,ABase* BaseToSpawn, ABuilding* BuildingToSpawn)
{
	if(PlayerStateRef->GetUnits() < PlayerStateRef->GetMaxUnits())
    {
		Multi_InitServerSpawn(UnitToSpawn, SpawnChosen, BaseToSpawn, BuildingToSpawn,PlayerStateRef);
		return Grid->GetGridData()->Find(SpawnChosen)->UnitOnTile != nullptr;
    }
	return false;

}

void ACustomPlayerController::Multi_InitServerSpawn_Implementation(EUnitType UnitToSpawn, FIntPoint SpawnChosen,
	ABase* BaseToSpawn, ABuilding* BuildingToSpawn, ACustomPlayerState* PlayerStat)
{
	if (PlayerStat->GetUnits() < PlayerStat->GetMaxUnits())
		Server_SpawnUnit(UnitToSpawn, SpawnChosen, BaseToSpawn, BuildingToSpawn, PlayerStat);
}

//spawn unit with server
void ACustomPlayerController::Server_SpawnUnit_Implementation(EUnitType UnitToSpawn, FIntPoint SpawnChosen , ABase* BaseToSpawn, ABuilding* BuildingToSpawn, ACustomPlayerState* PlayerStatRef)
{
	
	if (Grid->GetGridData()->Find(SpawnChosen)->UnitOnTile == nullptr)
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
			Multi_InitServerSpawn(UnitToSpawn, SpawnLoc, BaseToSpawn, nullptr, PlayerStateRef);
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
				Server_RessourceChange(PlayerStateRef,UnitToSpawn);
				GhostUnit = GetWorld()->SpawnActor<AGhostUnitSpawning>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
				GhostUnit->SetUnitType(UnitToSpawn);
				GhostUnit->Spawn();
				ActionMemory(EDC_ActionType::SpawnUnit);
			}
			break;
		case EUnitType::U_Tank:
			if(PlayerStateRef->GetStonePoints() >= PlayerStateRef->GetTankStoneCost() && PlayerStateRef->GetWoodPoints() >= PlayerStateRef->GetTankWoodCost() && PlayerStateRef->GetGoldPoints() >= PlayerStateRef->GetTankGoldCost())
			{
				Server_RessourceChange(PlayerStateRef,UnitToSpawn);
				GhostUnit = GetWorld()->SpawnActor<AGhostUnitSpawning>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
				GhostUnit->SetUnitType(UnitToSpawn);
				GhostUnit->Spawn();
				ActionMemory(EDC_ActionType::SpawnUnit);
			}
			break;
		case EUnitType::U_Mage:
			if(PlayerStateRef->GetStonePoints() >= PlayerStateRef->GetMageStoneCost() && PlayerStateRef->GetWoodPoints() >= PlayerStateRef->GetMageWoodCost() && PlayerStateRef->GetGoldPoints() >= PlayerStateRef->GetMageGoldCost())
			{
				Server_RessourceChange(PlayerStateRef,UnitToSpawn);
				GhostUnit = GetWorld()->SpawnActor<AGhostUnitSpawning>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
				GhostUnit->SetUnitType(UnitToSpawn);
				GhostUnit->Spawn();
				ActionMemory(EDC_ActionType::SpawnUnit);
			}
			break;
		case EUnitType::U_Leader:
			if(PlayerStateRef->GetStonePoints() >= PlayerStateRef->GetLeaderStoneCost() && PlayerStateRef->GetWoodPoints() >= PlayerStateRef->GetLeaderWoodCost() && PlayerStateRef->GetGoldPoints() >= PlayerStateRef->GetLeaderGoldCost())
			{
				Server_RessourceChange(PlayerStateRef,UnitToSpawn);
				GhostUnit = GetWorld()->SpawnActor<AGhostUnitSpawning>(Grid->GetGridData()->Find(SpawnChosen)->TileTransform.GetLocation(), FRotator(0,0,0));
				GhostUnit->SetUnitType(UnitToSpawn);
				GhostUnit->Spawn();
				ActionMemory(EDC_ActionType::SpawnUnit);
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
		Grid->GridVisual->AddStateToTile(SpawnChosen, EDC_TileState::Spawned);
	}
	Grid->GridVisual->RemoveStateFromTile(SpawnChosen, EDC_TileState::Spawnable);
}

void ACustomPlayerController::Server_RessourceChange_Implementation(const ACustomPlayerState* PSR,
                                                                    EUnitType UnitType)
{
	Multi_RessourceChange(PSR, UnitType);
}

	// ----------------------------
	// Cancel Actions

void ACustomPlayerController::Multi_RessourceChange_Implementation(const ACustomPlayerState* PSR,
	EUnitType UnitType)
{
	ACustomPlayerState* PlayerStateR = const_cast<ACustomPlayerState*>(PSR);
	switch (UnitType)
	{
	case EUnitType::U_Leader:
		PlayerStateR->ChangeWoodPoints(PlayerStateR->GetLeaderWoodCost(), false);
		PlayerStateR->ChangeStonePoints(PlayerStateR->GetLeaderStoneCost(), false);
		PlayerStateR->ChangeGoldPoints(PlayerStateR->GetLeaderGoldCost(), false);
		break;
	case EUnitType::U_Mage:
		PlayerStateR->ChangeWoodPoints(PlayerStateR->GetMageWoodCost(), false);
		PlayerStateR->ChangeStonePoints(PlayerStateR->GetMageStoneCost(), false);
		PlayerStateR->ChangeGoldPoints(PlayerStateR->GetMageGoldCost(), false);
		break;
	case EUnitType::U_Tank:
		PlayerStateR->ChangeWoodPoints(PlayerStateR->GetTankWoodCost(), false);
		PlayerStateR->ChangeStonePoints(PlayerStateR->GetTankStoneCost(), false);
		PlayerStateR->ChangeGoldPoints(PlayerStateR->GetTankGoldCost(), false);
		break;
	case EUnitType::U_Warrior:
		PlayerStateR->ChangeWoodPoints(PlayerStateR->GetWarriorWoodCost(), false);
		PlayerStateR->ChangeStonePoints(PlayerStateR->GetWarriorStoneCost(), false);
		PlayerStateR->ChangeGoldPoints(PlayerStateR->GetWarriorGoldCost(), false);
		break;
	default:
		break;
	}
	PlayerStateR->PlayerControllerRef->UpdateUi();
}

//cancel action (on final version)
void ACustomPlayerController::Server_CancelLastAction_Implementation()
{
	Multi_CancelLastAction();
}

void ACustomPlayerController::Multi_CancelLastAction_Implementation()
{
	if (!AllPlayerActions.IsEmpty() && PlayerStateRef->bIsActiveTurn)
	{
		// Get necessary reference for current cancelation 
		TObjectPtr<AUnit> ActorThatStops = Cast<AUnit>(AllPlayerActions.Last().ActorRef);
		if (!ActorThatStops)
			TObjectPtr<ATower> TowerThatStops = Cast<ATower>(AllPlayerActions.Last().ActorRef);
		
		switch(AllPlayerActions.Last().UnitAction)
		{
			case EDC_ActionType::None:
				break;
			case EDC_ActionType::UnitMove:
				RemoveLastActionMemory();
				ActorThatStops->Multi_CancelMove();
				PlayerStateRef->SetActionPoints(PlayerStateRef->GetActionPoints() + 1);
				if (UnitRef->bFirstActionIsMove)
					UnitRef->bFirstActionIsMove = false;
				break;
			case EDC_ActionType::UnitAttack:
				RemoveLastActionMemory();
				ActorThatStops->Multi_CancelAttack();
				PlayerStateRef->SetActionPoints(PlayerStateRef->GetActionPoints() + 1);
				break;
			case EDC_ActionType::TowerAttack:
				// CODE CANCEL FOR TOWER ATTACK
				break;
			case EDC_ActionType::DivineAssistance:
				// CODE CANCEL FOR DIVINE ASSISTANCE
				break;
			default: break;
		}
		AllPlayerActions.RemoveAt(AllPlayerActions.Num() - 1);
	}
	else if (!AllPlayerPassive.IsEmpty() && !PlayerStateRef->bIsActiveTurn)
	{
		AllPlayerPassive.Last().GhostRef->Destroy();
		Grid->GridVisual->RemoveStateFromTile(AllPlayerPassive.Last().TilePosition, EDC_TileState::Spawned);
		
		AllPlayerPassive.RemoveAt(AllPlayerPassive.Num() - 1);
	}

	RemoveLastActionMemory();
}

	// ----------------------------
	// Actions - End Turn

//call Action Active Turn with server
void ACustomPlayerController::Server_ActionActiveTurn_Implementation()
{
	Multi_ActionActiveTurnRedo();
}

//execute action active turn with multicast
void ACustomPlayerController::Multi_ActionActiveTurn_Implementation()
{
	/* DEPRECIATED
	if(IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("Multi_ActionActiveTurn"));
		HideWidget();
		//if PlayerState si set
		if(PlayerStateRef == nullptr)
			PlayerStateRef = Cast<ACustomPlayerState>(PlayerState);
		GetWorld()->GetTimerManager().ClearTimer(TimerActiveEndTurn);
		
		//if player is active
		if(PlayerStateRef->bIsActiveTurn)
		{
			DisableInput(this);
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
					TowerAction->CanAttack = TowerAction->IsGarrisoned;
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
					{
						Server_SpecialUnit(AllPlayerActions[0].UnitAttacking, BaseAction);
					}
					//if special action
					if(UnitAction)
					{
						Server_SpecialUnit(UnitAction,AllPlayerActions[0].UnitAttacking);
					}
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
	*/
}

void ACustomPlayerController::Multi_ActionActiveTurnRedo_Implementation()
{
	// Only do this once if this is the controller linked to the current session
	if (IsLocalController())
	{
		HideWidget();
		
		// Check if PlayerStateRef is set; set it if it isn't
		if(PlayerStateRef == nullptr)
			PlayerStateRef = Cast<ACustomPlayerState>(PlayerState);

		// Block Timer
		GetWorld()->GetTimerManager().ClearTimer(TimerActiveEndTurn);

		// If player is active (= is the right turn)
		if(PlayerStateRef->bIsActiveTurn)
		{
			// Stop the player from doing any other actions while the playback is happening
			DisableInput(this);

			// If there's still at least an action to do, do the first action in the list
			if(AllPlayerActions.Num() > 0)
			{
				switch (AllPlayerActions[0].UnitAction)
				{
					case EDC_ActionType::None: // Did Nothing ; shouldn't ever happen
						break;
					case EDC_ActionType::UnitMove: // Is the action this turn a movement?
						if(const TObjectPtr<AUnit> TurnUnitRef = Cast<AUnit>(AllPlayerActions[0].ActorRef))
						{
							ServerMoveUnit(TurnUnitRef);
							TurnUnitRef->bHasMoved = false;
						}
						break;
					case EDC_ActionType::UnitAttack: // Is the action this turn an attack initiated by a unit?
						if(const TObjectPtr<AUnit> TurnUnitRef = Cast<AUnit>(AllPlayerActions[0].ActorRef))
						{
							switch (AllPlayerActions[0].TargetType)
							{
							case EDC_TargetType::None: // Attacked nothing (Lol. Lmao even. L bozo. Territory Expansion: Ratio)
								break;
							case EDC_TargetType::Base: // Attacked the Enemy (hopefully) Base
								if (ABase* Target = Grid->GetGridData()->Find(AllPlayerActions[0].TargetLocation)->BaseOnTile)
									AttackBase(Target, TurnUnitRef);
								break;
							case EDC_TargetType::Building: // Attacked a Building
								if (ABuilding* Target = Grid->GetGridData()->Find(AllPlayerActions[0].TargetLocation)->BuildingOnTile)
									ServerAttackBuilding(Target, TurnUnitRef);
								break;
							case EDC_TargetType::Unit: // Attacked another Unit
								if (AUnit* Target = Grid->GetGridData()->Find(AllPlayerActions[0].TargetLocation)->UnitOnTile)
									ServerAttackUnit(Target, TurnUnitRef);
								break;
							default:; // Nothing ever happens
							}
							TurnUnitRef->bHasActed = false;
						}
						// Restart the action manager in 1 second
						GetWorld()->GetTimerManager().SetTimer(TimerActiveEndTurn, this, &ACustomPlayerController::Multi_ActionActiveTurnRedo, 1.f, false);

						break;
					case EDC_ActionType::TowerAttack: // Is the action this turn an attack initiated by a tower?
						// CODE ACTION : TOWER ATTACKING SOMEONE
						// (different now that the tower's attack
						// isn't from a unit that's stored there)
						
						break;
					case EDC_ActionType::DivineAssistance: // Is the action this turn the activation of the Divine Assistance? (Unlikely :p)
						// CODE ACTION : ACTIVATE DIVINE ASSISTANCE BUFF
						
						break;
					default: ;
				}
				AllPlayerActions.RemoveAt(0);	
			}
			else
			{
				// No actions are left
				AllPlayerActions.Empty();
				GetWorld()->GetTimerManager().ClearTimer(TimerActiveEndTurn);

				// Begin the Passive Actions initiated by the other player
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
			if (UnitRef->bHasActed && TempWarriorRef && UnitRef->GetFinalGhostMesh()->IsVisible())
			{
				PathReachable = Grid->GridPath->NewFindPath(Grid->ConvertLocationToIndex(UnitRef->GetFinalGhostMesh()->GetComponentLocation()), this);
			}
			else if(UnitRef->GetIsGarrison())
			{
				if (UnitRef->GetBuildingRef())
					PathReachable = UnitRef->GetBuildingRef()->MovementOptions;
			}
			else
				PathReachable = Grid->GridPath->NewFindPath(Grid->ConvertLocationToIndex(UnitRef->GetActorLocation()), this);

			for(FIntPoint Index : PathReachable)
			{
				Grid->GridVisual->AddStateToTile(Index, EDC_TileState::Reachable);
			}
		}
	
}

void ACustomPlayerController::RotateCamera(const FInputActionValue& Value)
{
	if(CameraPlayerRef)
	{
		CameraPlayerRef->RotateCamera(Value);
		InactivityTimer = 0;
		DisplayInputsOnHUD();
	}
}

void ACustomPlayerController::RotateCameraPitch(const FInputActionValue& Value)
{
	if(CameraPlayerRef)
	{
		CameraPlayerRef->RotateCameraPitch(Value);
		InactivityTimer = 0;
		DisplayInputsOnHUD();
	}
}

void ACustomPlayerController::ZoomCamera(const FInputActionValue& Value)
{
	if(CameraPlayerRef)
	{
		CameraPlayerRef->ZoomCamera(Value);
		InactivityTimer = 0;
		DisplayInputsOnHUD();
	}
}

void ACustomPlayerController::CancelAction(const FInputActionValue& Value)
{
	if(CameraPlayerRef)
	{
		CameraPlayerRef->CancelAction(Value);
		GEngine->AddOnScreenDebugMessage(-1,5.f, FColor::Red, TEXT("Nah I'd Win"));
		InactivityTimer = 0;
		DisplayInputsOnHUD();
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


void ACustomPlayerController::FailedTutorial_Implementation()
{
	
}


		// Stats

void ACustomPlayerController::DisplayWidget_Implementation()
{
	
}

void ACustomPlayerController::DisplayWidgetHovering_Implementation()
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

void ACustomPlayerController::HideWidget_Implementation()
{
}

	// ----------------------------
	// GETTERS

		// Turns

void ACustomPlayerController::RemoveLastActionMemory_Implementation()
{
}

void ACustomPlayerController::DisplayInputsOnHUD_Implementation()
{
}

void ACustomPlayerController::IncrementInactivityTimer()
{
	InactivityTimer++;
	DisplayInputsOnHUD();
}

bool ACustomPlayerController::GetIsInActiveTurn()
{
	return bIsInActiveTurn;
}

bool ACustomPlayerController::GetIsReady()
{
	return bIsReady;
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

void ACustomPlayerController::ActionMemory_Implementation(EDC_ActionType Action)
{
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
	bIsInActiveTurn = bA;
}

void ACustomPlayerController::SetIsReady(bool bR)
{
	bIsReady = bR;
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