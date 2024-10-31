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
#include "Upwall.h"
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
	
	GetWorld()->GetTimerManager().SetTimer(
		InitializationTimer, 
		this,
		&AUnit::AssignPlayerController_Implementation,
		0.2,
		true);
}

void AUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
		
	//if(HasAuthority())
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
	DOREPLIFETIME(AUnit, bIsCommandeerBuffed);
	DOREPLIFETIME(AUnit, PlayerControllerRef);
	DOREPLIFETIME(AUnit, bIsGhosts);
	DOREPLIFETIME(AUnit, PathToCross);
	DOREPLIFETIME(AUnit, PathToCrossPosition);
	DOREPLIFETIME(AUnit, bJustBecameGarrison);
	DOREPLIFETIME(AUnit, MoveSequencePos);
	DOREPLIFETIME(AUnit, FutureMovement);
	DOREPLIFETIME(AUnit, FutureMovementWithSpecial);
	DOREPLIFETIME(AUnit, HasActed);
	DOREPLIFETIME(AUnit, HasMoved);

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
	if(PlayerControllerRef)
		switch (UnitName)
		{
		case EUnitName::Leader:
			PlayerControllerRef->PlayerStateRef->SetLeaderCount(PlayerControllerRef->PlayerStateRef->GetLeaderCount() - 1);
			break;
		case EUnitName::Tank:
			PlayerControllerRef->PlayerStateRef->SetTankCount(PlayerControllerRef->PlayerStateRef->GetTankCount() - 1);
			break;
		case EUnitName::Warrior:
			PlayerControllerRef->PlayerStateRef->SetWarriorCount(PlayerControllerRef->PlayerStateRef->GetWarriorCount() - 1);
			break;
		case EUnitName::Mage:
			PlayerControllerRef->PlayerStateRef->SetMageCount(PlayerControllerRef->PlayerStateRef->GetMageCount() - 1);
			break;
		default:
			break;
		}
	Destroyed();
	GetWorld()->DestroyActor(this);
	if (GetWorld()->GetFirstPlayerController()->GetPlayerState<ACustomPlayerState>()->bIsInTutorial)
		DisplayWidgetTutorial();
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

void AUnit::AssignPlayerController_Implementation()
{
	if (HasAuthority())
	{
		if (GetPlayerOwner() == EPlayer::P_Neutral)
		{
			GetWorld()->GetTimerManager().ClearTimer(InitializationTimer);
			return;
		}
		TArray<AActor*> AllActors;
		TArray<ACustomPlayerController*> AllControllers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACustomPlayerController::StaticClass(), AllActors);
		if (!AllActors.IsEmpty())
		{
			for (AActor* CurrentActor : AllActors)
			{
				AllControllers.Add(Cast<ACustomPlayerController>(CurrentActor));
			}
			if (!AllControllers.IsEmpty()){
				for (ACustomPlayerController* CurrentPC : AllControllers)
				{
					if (CurrentPC->PlayerStateRef)
						if (CurrentPC->PlayerStateRef->PlayerTeam == GetPlayerOwner())
						{
							PlayerControllerRef = CurrentPC;
							PlayerControllerRef->OnTurnChangedDelegate.AddUFunction(this, "NewTurn");
							GetWorld()->GetTimerManager().ClearTimer(InitializationTimer);
							Server_GetBuffs();
							return;
						}
				}
			}
		}
	}
	else
		GetWorld()->GetTimerManager().ClearTimer(InitializationTimer);
}


// ----------------------------
// Interactions

bool AUnit::Interact_Implementation(ACustomPlayerController* PlayerController)
{

	PlayerControllerRef = PlayerController;
	return true;
}

void AUnit::DisplayWidgetTutorial()
{
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

	if (PlayerControllerRef)
		if (GetPlayerOwner() == EPlayer::P_Heaven && bIsCommandeerBuffed && PlayerControllerRef->PlayerStateRef->bIsActiveTurn)
		{
			CurrentHealth = UKismetMathLibrary::Clamp(CurrentHealth + 2, 0, MaxHealth +2);
		}
}


// ----------------------------
// Grid

void AUnit::Server_AddOnGrid_Implementation()
{
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
	FutureMovementWithSpecial = FutureMovement;

	GhostsFinaleLocationMesh->SetWorldLocation(Grid->ConvertIndexToLocation(FutureMovementPos));
	
	//InitGhosts();
	HasMoved = true;
}
// ----------------------------
// Movements

void AUnit::InitializeFullMove(TArray<FIntPoint> FullMove)
{
	if (FullMove.IsEmpty())
		return;
	
	// Initialisation
	PathToCross = FullMove;
	GhostsMesh->SetVisibility(false);
	GhostsFinaleLocationMesh->SetVisibility(false);
	bIsGhosts = false;
	bJustBecameGarrison = false;
	MoveSequencePos = 0;
	PathToCrossPosition = 0;

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

	while (true)
	{
		if(Grid->GetGridData()->Find(PathToCross.Last())->UnitOnTile && Grid->GetGridData()->Find(PathToCross.Last())->UnitOnTile != this)
		{
			if(PathToCross.Num() != 0)
				PathToCross.RemoveAt(PathToCross.Num()-1);
			else PathToCross.Empty();
		}
		else
		{
			break;
		}
	}
	TArray<FIntPoint> PathToCrossTemp;
	if(PathToCross.IsEmpty())
		return;

	for(FIntPoint index : PathToCross)
    {
    	if(Grid->GetGridData()->Find(index)->UnitOnTile != nullptr)
		{
			if(Grid->GetGridData()->Find(index)->UnitOnTile->GetPlayerOwner() != GetPlayerOwner())
            {
                break;
            }
    		
		}
		else if(Grid->GetGridData()->Find(index)->TowerOnTile)
		{
			if(Grid->GetGridData()->Find(index)->TowerOnTile->GetPlayerOwner() != GetPlayerOwner())
            {
                break;
            }
		}
		PathToCrossTemp.Add(index);
    }
	PathToCross = PathToCrossTemp;

	if (!PathToCross.IsEmpty())
		GetWorld()->GetTimerManager().SetTimer(
			MoveTimerHandle, 
			this,
			&AUnit::UnitMoveAnim,
			0.3,
			true);
}

void AUnit::UnitMoveAnim_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("UnitMoveAnim"));
	// Monte
	if (MoveSequencePos == 0)
	{
		SetActorLocation(GetActorLocation() + FVector(0,0,50));
		MoveSequencePos = 1;
	}
	
	// Déplacement
	else if (MoveSequencePos == 1 && PathToCrossPosition < PathToCross.Num() && PathToCrossPosition > -1 )
	{
		WillMove = true;
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
				if(PlayerControllerRef)
				{
					if (PlayerControllerRef->PlayerStateRef)
					{
						BuildingRef->SwitchOwner(PlayerControllerRef->PlayerStateRef);
					}
				}
				PathToCross.Empty();
				MoveSequencePos = 2;
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
		}/*
		//If Last Path is a unit on tile and the unit is not the unit moving
		else if (PathToCross[PathToCrossPosition] == PathToCross.Last())
		{
			if(Grid->GetGridData()->Find(PathToCross.Last())->UnitOnTile)
			{
				WillMove = false;
			}
		}*/
		if (WillMove)
			SetActorLocation(Grid->ConvertIndexToLocation(PathToCross[PathToCrossPosition]) + FVector(0,0,50));
		

		// If is last move
		if (PathToCross[PathToCrossPosition] == PathToCross.Last())
		{
			Grid->GridInfo->Multi_setUnitIndexOnGrid(PathToCross.Last(), this);
			PathToCross.Empty();
			PathToCrossPosition = 0;
			MoveSequencePos = 2;
		}
		
		
		PathToCrossPosition++;

		
	}

	// Descend
	else if (MoveSequencePos == 2)
	{
		SetActorLocation(GetActorLocation() + FVector(0,0,-50));
		MoveSequencePos = 0;
		if (PathToCross.IsEmpty())
		{
			GetWorldTimerManager().ClearTimer(MoveTimerHandle);

			// EndTurn
			if(PlayerControllerRef != nullptr)
			{
				FutureMovement.Empty();
				PlayerControllerRef->Server_ActionActiveTurn();
				Server_GetBuffs();

				if (TObjectPtr<AUnit_Child_Leader> Leader = Cast<AUnit_Child_Leader>(this))
					Leader->Server_PushBuff();
			}
		}
	}
	else PathToCrossPosition = 0;
}

void AUnit::MoveUnitEndTurn()
{
	Multi_HiddeGhosts();
	InitializeFullMove(FutureMovement);
	FutureMovement.Empty();
	Server_GetBuffs();
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
	GhostsMesh->SetVisibility(true);
	GhostsMesh->SetWorldLocation(GetActorLocation());
	GhostsFinaleLocationMesh->SetWorldLocation(Grid->ConvertIndexToLocation(FutureMovementPos));
	GhostsFinaleLocationMesh->SetVisibility(true);

	// Set ghost's position as unit's position on grid ;
	// Grid->GridInfo->Server_setUnitIndexOnGrid(FutureMovementPos, this);
	//bIsGhosts = true;
}

void AUnit::MoveGhosts(float DeltaTime)
{
	if(HasMoved)
		MoveGhostsAction(DeltaTime, FutureMovementWithSpecial);
		
	//Server_MoveGhosts(DeltaTime, FutureMovementWithSpecial);
}

void AUnit::Server_MoveGhosts_Implementation(float DeltaTime ,const TArray<FIntPoint> &PathToFollowGhost)
{
	
	//	MoveGhostsMulticast(DeltaTime, PathToFollowGhost);
}
void AUnit::MoveGhostsAction(float DeltaTime, const TArray<FIntPoint>& PathToFollowGhost)

{
	if(PathToFollowGhost.IsEmpty())
	{
		return;
	}
	if(HasActed)
	{
		if(Grid->GetGridData()->Find(FutureMovementWithSpecial.Last())->UpwallOnTile)
		{
			FutureMovementWithSpecial.Add(Grid->GetGridData()->Find(FutureMovementWithSpecial.Last())->UpwallOnTile->GetClimbLocation());
		}
	}
	
	if(PathToFollowGhost.Num() <= CurrentIndexGhost)
		CurrentIndexGhost = 0;
	
	GhostsMesh->SetWorldLocation(UKismetMathLibrary::VInterpTo_Constant(GhostsMesh->GetComponentLocation(), Grid->ConvertIndexToLocation(PathToFollowGhost[CurrentIndexGhost]), DeltaTime, 70.0f));
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
// Attack

void AUnit::TakeDamage(int Damage)
{
	if(bBuffTank && (Damage-(Defense+1)) > 0)
	{
		CurrentHealth -= Damage-(Defense+1);
	}
	else if((Damage-Defense) > 0)
		CurrentHealth -= (Damage-Defense);
}

void AUnit::AttackUnit(AUnit* UnitToAttack)
{
	if(UnitToAttack == nullptr || Grid == nullptr || UnitToAttack == this)
	{
		return;
	}
	if (Grid->GridPath->FindTileNeighbors(GetIndexPosition()).Contains(UnitToAttack->GetIndexPosition())
		|| UnitToAttack->IsGarrison)
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
			if (UnitToAttack->BuildingRef)
			{
				UnitToAttack->BuildingRef->UnitRef = nullptr;
				UnitToAttack->BuildingRef->GarrisonFull = false;
				UnitToAttack->IsGarrison = false;
				TArray<FIntPoint> MoveInBuilding = {UnitToAttack->IndexPosition};
				Grid->GridInfo->RemoveUnitInGrid(UnitToAttack);
				InitializeFullMove(MoveInBuilding);
			}
			Grid->GridInfo->RemoveUnitInGrid(UnitToAttack);
			UnitToAttack->Server_DestroyUnit();
		}
		if(GetCurrentHealth() < 1)
		{
			Grid->GridInfo->RemoveUnitInGrid(this);
			Grid->GridVisual->RemoveStateFromTile(IndexPosition, EDC_TileState::Selected);
			Server_DestroyUnit();
		}
		//if (PlayerControllerRef)
		//	PlayerControllerRef->VerifyBuildInteraction();
	}

}

void AUnit::AttackBase_Implementation(ABase* BaseToAttack)
{
	if(BaseToAttack == nullptr || Grid == nullptr)
	{
		return;
	}
	BaseToAttack->BaseTakeDamage(/*GetAttack()*/100);
	//if (PlayerControllerRef)
	//	PlayerControllerRef->VerifyBuildInteraction();
}

void AUnit::AttackBuilding_Implementation(ABuilding* BuildingToAttack)
{
	if (BuildingToAttack == nullptr || Grid == nullptr)
	{
		return;
	}
	if (BuildingToAttack->UnitRef)
		UnitToAttackRef = BuildingToAttack->UnitRef;
	AnimAttack(UnitToAttackRef);
	//if (PlayerControllerRef)
	//	PlayerControllerRef->VerifyBuildInteraction();
}

void AUnit::AnimAttack(AActor* ThingToAttack)
{
	if (!bBeganAttack && ThingToAttack)
	{
		UnitLocationInWorld = UnitMesh->GetComponentLocation();
		MoveTimerDelegate.BindUFunction(this, "AnimAttack", ThingToAttack);
		GetWorld()->GetTimerManager().SetTimer(
		MoveTimerHandle,
		MoveTimerDelegate,
		0.2,
		false);
		if (Cast<AUnit>(ThingToAttack)){
			AUnit* UnitToAttack = Cast<AUnit>(ThingToAttack);
			SetActorLocation(FVector(
				(UnitLocationInWorld.X + UnitToAttack->UnitMesh->GetComponentLocation().X) / 2,
				(UnitLocationInWorld.Y + UnitToAttack->UnitMesh->GetComponentLocation().Y) / 2,
				(UnitLocationInWorld.Z + UnitToAttack->UnitMesh->GetComponentLocation().Z) / 2)
				);
			UnitAttackAnimDelegate.BindUFunction(this, "AttackUnit", UnitToAttack);
			GetWorld()->GetTimerManager().SetTimer(
				UnitAttackAnimTimer,
				UnitAttackAnimDelegate,
				0.1,
				false);
		}
		else
		{
			SetActorLocation(FVector(
				(UnitLocationInWorld.X + ThingToAttack->GetActorLocation().X) / 2,
				(UnitLocationInWorld.Y + ThingToAttack->GetActorLocation().Y) / 2,
				(UnitLocationInWorld.Z + ThingToAttack->GetActorLocation().Z) / 2)
				);
		}
		bBeganAttack = true;
	}
	else
	{
			SetActorLocation(UnitLocationInWorld);
			bBeganAttack = false;
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

void AUnit::Server_GetBuffs_Implementation()
{
	Multi_GetBuffs();
}


void AUnit::Multi_GetBuffs_Implementation()
{
	if (bIsCommandeerBuffed) bIsCommandeerBuffed = false;
	if (bBuffTank) bBuffTank = false;
	for (FIntPoint CurrentLoc : Grid->GridPath->FindPath(GetIndexPosition(), FIntPoint(-999,-999), true, 3, false, false))
	{
		AUnit_Child_Leader* UnitToBuff = Cast<AUnit_Child_Leader> (Grid->GetGridData()->Find(CurrentLoc)->UnitOnTile);

		if (UnitToBuff && PlayerControllerRef)
		{
			if (GetPlayerOwner() == UnitToBuff->GetPlayerOwner())
			{
				bIsCommandeerBuffed = true;
				UnitToBuff = nullptr;
			}
		}
	}
	for (FIntPoint CurrentPos : Grid->GridPath->FindTileNeighbors(GetIndexPosition()))
	{
		if (CurrentPos != FIntPoint(-999,-999)){
			if (Grid->GetGridData()->Find(CurrentPos)){
				if (Grid->GetGridData()->Find(CurrentPos)->UnitOnTile)
				{
					if (Grid->GetGridData()->Find(CurrentPos)->UnitOnTile != this)
					{
						AUnit_Child_Tank* TankToCheck = Cast<AUnit_Child_Tank>(Grid->GetGridData()->Find(CurrentPos)->UnitOnTile);
						if (TankToCheck)
						{
							if (TankToCheck->GetIsUsingSpecial() && TankToCheck->GetPlayerOwner() == GetPlayerOwner()) bBuffTank = true;
						}
					}
				}
			}
		}
	}
}

// ----------------------------
// Cancel Actions


void AUnit::Server_CancelMove_Implementation()
{
	Multi_CancelMove();
}

void AUnit::Multi_CancelMove_Implementation()
{
	HasMoved = false;
	Multi_HiddeGhosts();
	FutureMovement.Empty();
}

void AUnit::Server_CancelAttack_Implementation()
{
	Multi_CancelAttack();
}

void AUnit::Multi_CancelAttack_Implementation()
{
	HasActed = false;
}

void AUnit::Server_CancelSpecial_Implementation()
{
	Multi_CancelSpecial();
}

void AUnit::Multi_CancelSpecial_Implementation()
{
	HasActed = false;
}




void AUnit::SetUnitIcon()
{
	if (PlayerOwner == EPlayer::P_Hell)
	{
		UnitIcon = HellIcon;
	}

	else if (PlayerOwner == EPlayer::P_Heaven)
	{
		UnitIcon = HeavenIcon;
	}

	else if (PlayerOwner == EPlayer::P_Neutral)
	{
		UnitIcon = NeutralIcon;
	}
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
void AUnit::SetPlayerOwner(EPlayer po)
{
	SetPlayerOwnerMulticast(po);
}

void AUnit::SetPlayerOwnerMulticast_Implementation(EPlayer po)
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
