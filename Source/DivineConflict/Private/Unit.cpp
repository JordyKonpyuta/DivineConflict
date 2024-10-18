// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit.h"
#include "Base.h"
#include "Building.h"
#include "CameraPlayer.h"
#include "CustomPlayerController.h"
#include "CustomPlayerState.h"
#include "Grid.h"
#include "GridInfo.h"
#include "GridPath.h"
#include "GridVisual.h"
#include "Tower.h"
#include "Unit_Child_Leader.h"
#include "Unit_Child_Tank.h"
#include "WidgetDamage2.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// INITIALISATIONS

AUnit::AUnit()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
	
	UnitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Unit Mesh"));
	SetRootComponent(UnitMesh);
	UnitMesh->SetStaticMesh( ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Game_Art/Asset_temp/Character/Paradis/tank_ange_pose.tank_ange_pose'")).Object);
	UnitMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	UnitMesh->SetIsReplicated(true);

	UnitName = EUnitName::Tank;
	
	AllMaterials.Add(ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("/Script/Engine.Material'/Game/Core/Texture_DEBUG/M_NeutralPlayer.M_NeutralPlayer'")).Object);
	AllMaterials.Add(ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Core/Texture_DEBUG/Mi_HeavenPlayer.Mi_HeavenPlayer'")).Object);
	AllMaterials.Add(ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Core/Texture_DEBUG/Mi_HellPlayer.Mi_HellPlayer'")).Object);

	MaterialToGhosts = ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("/Script/Engine.Material'/Game/Core/Texture/M_Ghosts.M_Ghosts'")).Object;

	GhostsMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ghosts Mesh"));
	GhostsMesh->SetStaticMesh(UnitMesh->GetStaticMesh());
	GhostsMesh->SetMaterial(0, MaterialToGhosts);
	GhostsMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	GhostsMesh->SetVisibility(false);

	GhostsFinaleLocationMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ghosts Finale Location Mesh"));
	GhostsFinaleLocationMesh->SetStaticMesh(UnitMesh->GetStaticMesh());
	GhostsFinaleLocationMesh->SetMaterial(0, MaterialToGhosts);
	GhostsFinaleLocationMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	GhostsFinaleLocationMesh->SetVisibility(false);

	// Widget Damage
	/*DamageWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetDamage"));
	static ConstructorHelpers::FClassFinder<UWidgetDamage2> classWidgetDamage(TEXT("/Game/Core/Blueprints/Widget/Units/WBP_WidgetDamage"));
	if (classWidgetDamage.Succeeded()) DamageWidgetComponent->SetWidgetClass(classWidgetDamage.Class);*/
}

void AUnit::BeginPlay()
{
	Super::BeginPlay();

	if(Grid != nullptr)
	{
		if (Grid->GridInfo == nullptr)
		{
			return;
		}


		//Grid->GridInfo->AddUnitInGrid(Grid->ConvertLocationToIndex(GetActorLocation()), this);
		//Timer 2s
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AUnit::Server_AddOnGrid_Implementation, 2.0f, false);

		//Grid->GridInfo->AddUnitInGrid(Grid->ConvertLocationToIndex(GetActorLocation()), this);
		Server_AddOnGrid();
		
		switch (PlayerOwner)
		{
		case EPlayer::P_Hell:
			UnitMesh->SetMaterial(0, AllMaterials[2]);
			break;
		case EPlayer::P_Heaven:
			UnitMesh->SetMaterial(0, AllMaterials[1]);
			break;
		case EPlayer::P_Neutral:
			UnitMesh->SetMaterial(0, AllMaterials[0]);
			break;
		}
		FTimerHandle timerTestOnTower;
		GetWorld()->GetTimerManager().SetTimer(timerTestOnTower, this, &AUnit::testOnTower, 1.0f, false);
	}
	else 
    {
		SetGrid();
    }
	GhostsMesh->SetStaticMesh(UnitMesh->GetStaticMesh());
	GhostsFinaleLocationMesh->SetStaticMesh(UnitMesh->GetStaticMesh());
	
	if (DamageWidgetComponent)
	{
		DamageWidgetComponent->AttachToComponent(UnitMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("DamageWidgetComponent")));
	}
	

}

void AUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
		
	if(HasAuthority())
		MoveGhosts(DeltaTime);

	// Widget Damage
	/*if (DamageWidgetComponent && PlayerControllerRef)
	{
		if (PlayerControllerRef->CameraPlayerRef)
		{
			DamageWidgetComponent->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(GetTargetLocation(), PlayerControllerRef->CameraPlayerRef->FollowCamera->GetComponentLocation()));
		} 
	}*/
}

void AUnit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUnit, UnitMesh);
	DOREPLIFETIME(AUnit, PlayerOwner);
	DOREPLIFETIME(AUnit, bIsClimbing);
	DOREPLIFETIME(AUnit, CurrentHealth);
	DOREPLIFETIME(AUnit, IsGarrison);
	DOREPLIFETIME(AUnit, PM);
	DOREPLIFETIME(AUnit, bBuffTank);
	DOREPLIFETIME(AUnit, bIsGhosts);
	DOREPLIFETIME(AUnit, PathToCross);
	DOREPLIFETIME(AUnit, PathToCrossPosition);
	DOREPLIFETIME(AUnit, bJustBecameGarrison);
	DOREPLIFETIME(AUnit, MoveSequencePos);
	DOREPLIFETIME(AUnit, FutureMovement);
	DOREPLIFETIME(AUnit, HasActed)

}

// ----------------------------
// Initial Tests

void AUnit::testOnTower()
{
	if(Grid)
	{
		if(Grid->GetGridData()->Find(IndexPosition) != nullptr)
		{
			if ( Grid->GetGridData()->Find(IndexPosition)->TowerOnTile)
			{
				Grid->GetGridData()->Find(IndexPosition)->TowerOnTile->UnitInGarrison = this;
				Grid->GetGridData()->Find(IndexPosition)->TowerOnTile->IsGarrisoned = true;
				TowerRef = Grid->GetGridData()->Find(IndexPosition)->TowerOnTile;
			}

		}
	}
}

void AUnit::SetGrid()
{
	//get the grid
	AActor* FoundActors = UGameplayStatics::GetActorOfClass(GetWorld(), AGrid::StaticClass());
	if (FoundActors != nullptr)
	{
		Grid = Cast<AGrid>(FoundActors);
		Grid->GridInfo->AddUnitInGrid(Grid->ConvertLocationToIndex(GetActorLocation()), this);
		testOnTower();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Grid Found"));
		//delay 0.2s and try again
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AUnit::SetGrid, 0.2f, false);
		
	}
}

// ----------------------------
// Overriden & Not in any category

void AUnit::Destroyed()
{
		
	Super::Destroyed();
}

void AUnit::Server_DestroyUnit_Implementation()
{
	Grid->GridInfo->RemoveUnitInGrid(this);
	Destroyed();
	GetWorld()->DestroyActor(this);
}

// Called to bind functionality to input
void AUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AUnit::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
	SetIsSelected(true);
}


// ----------------------------
// Interactions

bool AUnit::Interact_Implementation(ACustomPlayerController* PlayerController)
{

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Interact unit"));
	PlayerControllerRef = PlayerController;
	//PlayerControllerRef->CameraPlayerRef->IsMovingUnit = true;
	//DisplayWidget();


	
	return true;
}

void AUnit::interaction(ACustomPlayerController* PlayerController)
{
}

// ----------------------------
// Turn Switch

void AUnit::NewTurn()
{
	SetIsSelected(false);
	bIsClimbing = false;
	SetBuffTank(false);
	
}


// ----------------------------
// Grid

void AUnit::Server_AddOnGrid_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Unit Add on grid"));
	Grid->GridInfo->AddUnitInGrid(Grid->ConvertLocationToIndex(GetActorLocation()), this);
}

void AUnit::SetGrid(AGrid* NewGrid)
{
	Grid = NewGrid;
}

// ----------------------------
// Prepare Moves

void AUnit::Multi_PrepareMove_Implementation(const TArray<FIntPoint>& NewPos)
{
	FutureMovement = NewPos;
	FutureMovementPos = FutureMovement.Last();
	InitGhosts();
	//Grid->GridInfo->Server_setUnitIndexOnGrid(IndexPosition,this);
	//PlayerControllerRef->AllPlayerActions.Add(FStructActions(this, EDC_ActionPlayer::MoveUnit));
}

// ----------------------------
// Movements

void AUnit::Move_Implementation(const TArray<FIntPoint> &PathIn)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Unit moved"));
	GhostsMesh->SetVisibility(false);
	GhostsFinaleLocationMesh->SetVisibility(false);
	bIsGhosts = false;
	bJustBecameGarrison = false;
	Path.Empty();
	Path = FutureMovement;
	if (Path.Num()!=-1)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Unit Path Ready"));
		for(FIntPoint index : Path)
		{
			if (index == Path.Last() && Grid->GetGridData()->Find(index)->UnitOnTile)
			{
				Grid->GridVisual->RemoveStateFromTile(index, EDC_TileState::Pathfinding);
				break;
			}
			if (Grid->GetGridData()->Find(index)->BuildingOnTile)
			{
				if (Grid->GetGridData()->Find(index)->BuildingOnTile->GarrisonFull != true)
				{

					SetActorLocation(Grid->GetGridData()->Find(index)->BuildingOnTile->GetActorLocation());
					Grid->GetGridData()->Find(index)->BuildingOnTile->UnitRef = this;
					Grid->GetGridData()->Find(index)->BuildingOnTile->GarrisonFull = true;

					Grid->GridVisual->RemoveStateFromTile(index, EDC_TileState::Pathfinding);
					SetIsGarrison(true);
					bJustBecameGarrison = true;
					BuildingRef = Grid->GetGridData()->Find(index)->BuildingOnTile;
					if (PlayerControllerRef->PlayerStateRef != nullptr)
					{
						BuildingRef->SwitchOwner(PlayerControllerRef->PlayerStateRef);
					}
					break;
				}
				else
				{
					if (this != Grid->GetGridData()->Find(index)->BuildingOnTile->UnitRef)
					{
						for(FIntPoint Superindex : Path)
						{
							Grid->GridVisual->RemoveStateFromTile(Superindex, EDC_TileState::Pathfinding);
						}
						Grid->GridVisual->RemoveStateFromTile(index, EDC_TileState::Pathfinding);
						break;
					}
				}
			}

			else if (Grid->GetGridData()->Find(index)->TowerOnTile)
			{
				if (Grid->GetGridData()->Find(index)->TowerOnTile->IsGarrisoned == false)
				{
					SetActorLocation(Grid->GetGridData()->Find(index)->TowerOnTile->GetActorLocation());
					Grid->GetGridData()->Find(index)->TowerOnTile->UnitInGarrison = this;
					Grid->GetGridData()->Find(index)->TowerOnTile->IsGarrisoned = true;

					Grid->GridVisual->RemoveStateFromTile(index, EDC_TileState::Pathfinding);
					SetIsGarrison(true);
					bJustBecameGarrison = true;
				}
			}
			FVector location = Grid->ConvertIndexToLocation(index);
			Grid->GridVisual->RemoveStateFromTile(index, EDC_TileState::Pathfinding);
			SetActorLocation(location);
		}
		
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Unit moved"));
		
		Grid->GridInfo->Multi_setUnitIndexOnGrid(Grid->ConvertLocationToIndex(GetActorLocation()),this);

		if (IsGarrison && !bJustBecameGarrison)
		{
			BuildingRef->UnitRef = nullptr;
			BuildingRef->GarrisonFull = false;
			IsGarrison = false;
			BuildingRef = nullptr;
		}
	}
	if(PlayerControllerRef != nullptr)
    {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Next Action"));
        PlayerControllerRef->ActionEndTurn();
    }
}

void AUnit::Server_Move_Implementation(const TArray<FIntPoint>& PathToFollow)
{
	Move(PathToFollow);
}

void AUnit::InitializeFullMove(TArray<FIntPoint> FullMove)
{
	// Initialisation
	PathToCross = FullMove;
	GhostsMesh->SetVisibility(false);
	GhostsFinaleLocationMesh->SetVisibility(false);
	bIsGhosts = false;
	bJustBecameGarrison = false;
	MoveSequencePos = 0;

	if (IsGarrison && !bJustBecameGarrison)
	{
		if(BuildingRef)
		{
			BuildingRef->UnitRef = nullptr;
			BuildingRef->GarrisonFull = false;
		}
		if(TowerRef)
		{
			TowerRef->UnitInGarrison = nullptr;
			TowerRef->IsGarrisoned = false;
		}
		IsGarrison = false;
		BuildingRef = nullptr;
	}
		
	GetWorld()->GetTimerManager().SetTimer(
		MoveTimerHandle, 
		this,
		&AUnit::UnitMoveAnim,
		0.3,
		true);
}

void AUnit::UnitMoveAnim_Implementation()
{
	// Monte
	if (MoveSequencePos == 0)
	{
		SetActorLocation(GetActorLocation() + FVector(0,0,50));
		MoveSequencePos++;
	}

	// Déplacement
	else if (MoveSequencePos == 1)
	{
		bool WillMove = true;
		Grid->GridVisual->RemoveStateFromTile(PathToCross[PathToCrossPosition], EDC_TileState::Pathfinding);

		// If you cross a building
		if (Grid->GetGridData()->Find(PathToCross[PathToCrossPosition])->BuildingOnTile)
		{
			// If the building is empty
			if (Grid->GetGridData()->Find(PathToCross[PathToCrossPosition])->BuildingOnTile->GarrisonFull != true)
			{
				// Set Unit's visual Location
				SetActorLocation(Grid->GetGridData()->Find(PathToCross[PathToCrossPosition])->BuildingOnTile->GetActorLocation());
				SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 50));
				Grid->GetGridData()->Find(PathToCross[PathToCrossPosition])->BuildingOnTile->UnitRef = this;
				Grid->GetGridData()->Find(PathToCross[PathToCrossPosition])->BuildingOnTile->GarrisonFull = true;

				// Remove Pathfinding Artifacts
				Grid->GridVisual->RemoveStateFromTile(PathToCross[PathToCrossPosition], EDC_TileState::Pathfinding);

				// Set all necessary values
				SetIsGarrison(true);
				bJustBecameGarrison = true;
				BuildingRef = Grid->GetGridData()->Find(PathToCross[PathToCrossPosition])->BuildingOnTile;
				if (PlayerControllerRef->PlayerStateRef)
				{
					BuildingRef->SwitchOwner(PlayerControllerRef->PlayerStateRef);
				}
			}
			// If the building is full
			else
			{
				if (this != Grid->GetGridData()->Find(PathToCross[PathToCrossPosition])->BuildingOnTile->UnitRef)
				{
					for(const FIntPoint SuperIndex : Path)
					{
						Grid->GridVisual->RemoveStateFromTile(SuperIndex, EDC_TileState::Pathfinding);
					}
					Grid->GridVisual->RemoveStateFromTile(PathToCross[PathToCrossPosition], EDC_TileState::Pathfinding);

					for (const FIntPoint AnFIntPoint : PathToCross)
					{
						Grid->GridVisual->RemoveStateFromTile(AnFIntPoint, EDC_TileState::Pathfinding);
					}
				}
			}
			FIntPoint EarlyLastPath = Grid->ConvertLocationToIndex(GetActorLocation());
			PathToCross.Empty();
			PathToCross.Add(EarlyLastPath);
			PathToCrossPosition = 0;
			WillMove = false;
		}

		// If you cross a tower
		else if (Grid->GetGridData()->Find(PathToCross[PathToCrossPosition])->TowerOnTile)
		{
			if (Grid->GetGridData()->Find(PathToCross[PathToCrossPosition])->TowerOnTile->IsGarrisoned == false)
			{
				SetActorLocation(Grid->GetGridData()->Find(PathToCross[PathToCrossPosition])->TowerOnTile->GetActorLocation());
				Grid->GetGridData()->Find(PathToCross[PathToCrossPosition])->TowerOnTile->UnitInGarrison = this;
				Grid->GetGridData()->Find(PathToCross[PathToCrossPosition])->TowerOnTile->IsGarrisoned = true;
				TowerRef = Grid->GetGridData()->Find(PathToCross[PathToCrossPosition])->TowerOnTile;

				Grid->GridVisual->RemoveStateFromTile(PathToCross[PathToCrossPosition], EDC_TileState::Pathfinding);
				SetIsGarrison(true);
				bJustBecameGarrison = true;
			}
		}
		//If Last Path is a unit on tile and the unit is not the unit moving
		else if (PathToCross[PathToCrossPosition] == PathToCross.Last())
		{
			if(Grid->GetGridData()->Find(PathToCross.Last())->UnitOnTile)
			{
				UE_LOG( LogTemp, Warning, TEXT("Unit moved to last position"));
				WillMove = false;
			}
		}

		UE_LOG( LogTemp, Warning, TEXT("WillMove : %d"), WillMove);
		if (WillMove)
			SetActorLocation(Grid->ConvertIndexToLocation(PathToCross[PathToCrossPosition]) + FVector(0,0,50));
		
		

		// If is last move
		if (PathToCross[PathToCrossPosition] == PathToCross.Last())
		{
			UE_LOG( LogTemp, Warning, TEXT("Unit moved to last position"));
			//Grid->GridVisual->RemoveStateFromTile(PathToCross[PathToCrossPosition], EDC_TileState::Pathfinding);
			Grid->GridInfo->Multi_setUnitIndexOnGrid(PathToCross.Last(), this);
			PathToCross.Empty();
			PathToCrossPosition = 0;
			
		}
		
		

		MoveSequencePos++;
	}

	// Descend
	else if (MoveSequencePos == 2)
	{
		SetActorLocation(GetActorLocation() + FVector(0,0,-50));
		PathToCrossPosition++;
		MoveSequencePos = 0;
		if (PathToCross.IsEmpty())
		{
			GetWorldTimerManager().ClearTimer(MoveTimerHandle);

			// EndTurn
			if(PlayerControllerRef != nullptr)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Next Action"));
				FutureMovement.Empty();
				PlayerControllerRef->ActionEndTurn();
			}
		}
	}
}

void AUnit::MoveUnitEndTurn()
{
	Multi_HiddeGhosts();
	InitializeFullMove(FutureMovement);
	FutureMovement.Empty();
	GetBuffs();
}

// ----------------------------
// Ghosts

void AUnit::Multi_HiddeGhosts_Implementation()
{
	GhostsMesh->SetVisibility(false);
	GhostsFinaleLocationMesh->SetVisibility(false);
	bIsGhosts = false;
}

UStaticMeshComponent* AUnit::GetFinalGhostMesh()
{
	return GhostsFinaleLocationMesh;
}

void AUnit::InitGhosts_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Init Ghosts"));
	GhostsMesh->SetVisibility(true);
	GhostsMesh->SetWorldLocation(GetActorLocation());
	GhostsFinaleLocationMesh->SetWorldLocation(Grid->ConvertIndexToLocation(FutureMovementPos));
	GhostsFinaleLocationMesh->SetVisibility(true);

	// Set ghost's position as unit's position on grid
	//Grid->GridInfo->Server_setUnitIndexOnGrid(FutureMovementPos, this);
	bIsGhosts = true;
}

void AUnit::MoveGhosts(float DeltaTime)
{

	Server_MoveGhosts(DeltaTime, FutureMovement);
}

void AUnit::Server_MoveGhosts_Implementation(float DeltaTime ,const TArray<FIntPoint> &PathToFollowGhost)
{
	if(bIsGhosts)
		MoveGhostsMulticast(DeltaTime, PathToFollowGhost);
}

void AUnit::MoveGhostsMulticast_Implementation(float DeltaTime,const TArray<FIntPoint> &PathToFollowGhost)
{
	if(PathToFollowGhost.IsEmpty())
	{
		return;
	}
	if(PathToFollowGhost.Num() <= CurrentIndexGhost)
		CurrentIndexGhost = 0;
	
	GhostsMesh->SetWorldLocation(UKismetMathLibrary::VInterpTo_Constant(GhostsMesh->GetComponentLocation(), Grid->ConvertIndexToLocation(PathToFollowGhost[CurrentIndexGhost]), DeltaTime, 70.0f));
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, TEXT("Move Ghosts"));
	if(GhostsMesh->GetComponentLocation() == Grid->ConvertIndexToLocation(PathToFollowGhost[CurrentIndexGhost]))
	{
		CurrentIndexGhost++;
		if(PathToFollowGhost.Num() == CurrentIndexGhost)
		{
			CurrentIndexGhost = 0;
			GhostsMesh->SetWorldLocation(GetActorLocation());
			GhostsFinaleLocationMesh->SetVisibility(true);
		}
	}
}

// ----------------------------
// Prepare Attacks

void AUnit::PrepareAttackUnit(FIntPoint AttackPos)
{
	if (Grid->GetGridData()->Find(AttackPos)->UnitOnTile)
	{
		UnitToAttackRef = Grid->GetGridData()->Find(AttackPos)->UnitOnTile;
	}
}

void AUnit::PrepareAttackBuilding(FIntPoint AttackPos)
{
	if (Grid->GetGridData()->Find(AttackPos)->BuildingOnTile && PlayerControllerRef->CurrentPA > 0)
	{
		BuildingToAttackRef = Grid->GetGridData()->Find(AttackPos)->BuildingOnTile;
		if (BuildingToAttackRef->UnitRef && (BuildingToAttackRef->UnitRef->GetUnitTeam() != PlayerControllerRef->PlayerStateRef->PlayerTeam))
		{
			UnitToAttackRef = BuildingToAttackRef->UnitRef;
			PlayerControllerRef->AllPlayerActions.Add(FStructActions(this, EDC_ActionPlayer::AttackBuilding));
			PlayerControllerRef->CurrentPA--;
		}
	}
}

void AUnit::PrepareAttackBase(FIntPoint AttackPos)
{
	if (Grid->GetGridData()->Find(AttackPos)->BaseOnTile && PlayerControllerRef->CurrentPA > 1)
	{
		EnemyBase = Grid->GetGridData()->Find(AttackPos)->BaseOnTile;
		PlayerControllerRef->CurrentPA--;
	}
}

// ----------------------------
// Attack

void AUnit::TakeDamage(int Damage)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("UNIT TAKE DAMAGE"));
	if(bBuffTank && (Damage-(Defense+1)) > 0)
	{
		CurrentHealth -= (Damage-(Defense+1));
		/*if (DamageWidgetComponent != nullptr){
			if (dynamic_cast<UWidgetDamage2*>(DamageWidgetComponent))
			{
				Cast<UWidgetDamage2>(DamageWidgetComponent)->ChangeTextDmg(Damage-(Defense+1));
			}
		}*/
	}
	else if((Damage-Defense) > 0)
		CurrentHealth -= (Damage-Defense);
}

void AUnit::AttackUnit(AUnit* UnitToAttack)
{
	//AUnit* UnitToAttack = UnitToAttackRef;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("UnitToAttack : ") + UnitToAttack->GetName());
	if(UnitToAttack == nullptr || Grid == nullptr || UnitToAttack == this)
	{
		return;
	}
	if (Grid->GridPath->FindTileNeighbors(GetIndexPosition()).Contains(UnitToAttack->GetIndexPosition())
		|| Grid->GetGridData()->Find(UnitToAttack->GetIndexPosition())->BuildingOnTile)
	{
		if (PlayerOwner == EPlayer::P_Hell && bIsCommandeerBuffed)
		{
			UnitToAttack->TakeDamage(GetAttack() + 1);
		} else
		{
			UnitToAttack->TakeDamage(GetAttack());
		}

		if (UnitToAttack->PlayerOwner == EPlayer::P_Hell && UnitToAttack->bIsCommandeerBuffed)
		{
			TakeDamage(UnitToAttack->GetAttack() + 1);
		} else
		{
			TakeDamage(UnitToAttack->GetAttack());
		}

		if(UnitToAttack->GetCurrentHealth() < 1)
		{
			Grid->GridInfo->RemoveUnitInGrid(UnitToAttack);
			//PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->SetUnits(PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->GetUnits() - 1);
			if (UnitToAttack->BuildingRef)
			{
				UnitToAttack->BuildingRef->UnitRef = nullptr;
				UnitToAttack->BuildingRef->GarrisonFull = false;
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Unit Got Killed and removed")));
				if(GetCurrentHealth() < 1)
				{
					FutureMovement.Insert(UnitToAttack->GetIndexPosition(), 0);
					Move(FutureMovement);
				}
			}
			UnitToAttack->Server_DestroyUnit();
			/*if(GetCurrentHealth() < 1)
			{
				Grid->GridInfo->RemoveUnitInGrid(this);
				Grid->GridVisual->RemoveStateFromTile(IndexPosition, EDC_TileState::Selected);
				//PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->SetUnits(PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->GetUnits() - 1);
				Destroyed();
			}*/
		}
		if(GetCurrentHealth() < 1)
		{
			Grid->GridInfo->RemoveUnitInGrid(this);
			Grid->GridVisual->RemoveStateFromTile(IndexPosition, EDC_TileState::Selected);
			//PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->SetUnits(PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->GetUnits() - 1);
			Server_DestroyUnit();
		}
	}

}

void AUnit::AttackBase_Implementation(ABase* BaseToAttack)
{
	if(BaseToAttack == nullptr || Grid == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BaseToAttack is null"));
		return;
	}
	BaseToAttack->TakeDamage(/*GetAttack()*/100);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("health Base: ") + FString::FromInt(BaseToAttack->GetHealth()));
}

void AUnit::AttackBuilding_Implementation(ABuilding* BuildingToAttack)
{
	if (BuildingToAttack == nullptr || Grid == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BuildingToAttack is null"));
		return;
	}
	UnitToAttackRef = BuildingToAttack->UnitRef;
	AnimAttack();
}

void AUnit::AnimAttack()
{
	if (!bBeganAttack)
	{
		UnitLocationInWorld = UnitMesh->GetComponentLocation();
		SetActorLocation(FVector(
			(UnitLocationInWorld.X + UnitToAttackRef->UnitMesh->GetComponentLocation().X) / 2,
			(UnitLocationInWorld.Y + UnitToAttackRef->UnitMesh->GetComponentLocation().Y) / 2,
			(UnitLocationInWorld.Z + UnitToAttackRef->UnitMesh->GetComponentLocation().Z) / 2));
		GetWorld()->GetTimerManager().SetTimer(
			MoveTimerHandle, 
			this,
			&AUnit::AnimAttack,
			0.2,
			false);
		AttackUnit(UnitToAttackRef);
		bBeganAttack = true;
	} else
	{
		SetActorLocation(UnitLocationInWorld);
		bBeganAttack = false;
	}
}

// ----------------------------
// Prepare Specials

void AUnit::PrepareSpecial(FIntPoint SpecialPos)
{
	if (Grid->GetGridData()->Find(SpecialPos)->UnitOnTile && PlayerControllerRef->CurrentPA > 1)
	{
		UsedSpecial = true;
		PlayerControllerRef->CurrentPA--;
	}
}

// ----------------------------
// Specials

void AUnit::Special()
{
	
}

void AUnit::SpecialUnit(AUnit* UnitToAttack)
{
}

void AUnit::SpecialBase(ABase* BaseToAttack)
{
}

// ----------------------------
// Get Buffs

void AUnit::GetBuffs()
{
	if (bIsCommandeerBuffed) bIsCommandeerBuffed = false;
	if (bBuffTank) bBuffTank = false;
	for (FIntPoint CurrentLoc : Grid->GridPath->FindPath(GetIndexPosition(), FIntPoint(-999,-999), true, 3, false))
	{
		AUnit_Child_Leader* UnitToBuff = Cast<AUnit_Child_Leader> (Grid->GetGridData()->Find(CurrentLoc)->UnitOnTile);
		if (UnitToBuff)
		{
			bIsCommandeerBuffed = true;
			UnitToBuff = nullptr;
		}
	}
	for (FIntPoint CurrentPos : Grid->GridPath->FindTileNeighbors(GetIndexPosition()))
	{
		AUnit_Child_Tank* TankToCheck = Cast<AUnit_Child_Tank>(Grid->GetGridData()->Find(CurrentPos)->UnitOnTile);
		if (TankToCheck)
		{
			if (TankToCheck->GetIsUsingSpecial()) bBuffTank = true;
		}
	}
}

// ----------------------------
// Cancel Actions

void AUnit::CancelMove()
{
	HasMoved = false;
	Multi_HiddeGhosts();
	FutureMovement.Empty();
}

void AUnit::CancelAttack()
{
	HasActed = false;
}

void AUnit::CancelSpecial()
{
	HasActed = false;
}

// ----------------------------
// GETTERS

// Static Meshes
UStaticMeshComponent* AUnit::GetStaticMesh()
{
	return UnitMesh;
}

// References
ABuilding* AUnit::GetBuildingRef()
{
	return BuildingRef;
}

// Int
int AUnit::GetMaxHealth()
{
	return MaxHealth;
}

int AUnit::GetCurrentHealth()
{
	return CurrentHealth;
}

int AUnit::GetPM()
{
	return PM;
}

int AUnit::GetAttack()
{
	return Attack;
}

int AUnit::GetDefense()
{
	return Defense;
}

// Bool
bool AUnit::GetIsSelected()
{
	return IsSelected;
}

bool AUnit::GetIsClimbing()
{
	return bIsClimbing;
}

bool AUnit::GetBuffTank()
{
	return bBuffTank;
}

bool AUnit::GetIsGarrison()
{
	return IsGarrison;
}

bool AUnit::GetIsCommandeerBuffed()
{
	return bIsCommandeerBuffed;
}

// Enums
EPlayer AUnit::GetUnitTeam()
{
	return PlayerOwner;
}

EPlayer AUnit::GetPlayerOwner()
{
	return PlayerOwner;
}

// String
FString AUnit::GetNameUnit()
{
	return Name;
}


// FIntPoint
FIntPoint AUnit::GetIndexPosition()
{
	return IndexPosition;
}

// ----------------------------
// SETTERS

// References
void AUnit::SetBuildingRef(ABuilding* Building)
{
	BuildingRef = Building;
}

// Int
void AUnit::SetMaxHealth(int mh)
{
	MaxHealth = mh;
}

void AUnit::SetCurrentHealth(int ch)
{
	CurrentHealth = ch;
}

void AUnit::SetPM(int p)
{
	PM = p;
}

void AUnit::SetAttack(int a)
{
	Attack = a;
}

void AUnit::SetDefense(int d)
{
	Defense = d;
}

// Bool
void AUnit::SetIsSelected(bool s)
{
	IsSelected = s;
}

void AUnit::SetIsGarrison(bool bG)
{
	IsGarrison = bG;
}

void AUnit::SetIsClimbing(bool ic)
{
	bIsClimbing = ic;
}

void AUnit::SetBuffTank(bool bt)
{
	bBuffTank = bt;
}

void AUnit::SetIsCommandeerBuffed(bool bC)
{
	bIsCommandeerBuffed = bC;
}

// Enums
void AUnit::SetUnitTeam(EPlayer PO)
{
	PlayerOwner = PO;
}

void AUnit::SetPlayerOwner(EPlayer po)
{
	PlayerOwner = po;
	
	switch (PlayerOwner)
	{
	case EPlayer::P_Hell:
		UnitMesh->SetMaterial(0, AllMaterials[2]);
		break;
	case EPlayer::P_Heaven:
		UnitMesh->SetMaterial(0, AllMaterials[1]);
		break;
	case EPlayer::P_Neutral:
		UnitMesh->SetMaterial(0, AllMaterials[0]);
		break;
	}
}

// FString
void AUnit::SetName(FString n)
{
	Name = n;
}

// FIntPoint
void AUnit::SetIndexPosition(FIntPoint ip)
{
	IndexPosition = ip;
}

// ----------------------------
// Messy Hell (We need to put this in the correct Sections!)
