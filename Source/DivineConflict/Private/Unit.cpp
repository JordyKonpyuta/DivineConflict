// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit.h"

#include "Base.h"
#include "Building.h"
#include "CustomPlayerController.h"
#include "CustomPlayerState.h"
#include "Grid.h"
#include "GridInfo.h"
#include "GridVisual.h"
#include "Tower.h"
#include "Kismet/GameplayStatics.h"
#include <chrono>
#include <thread>

#include "VectorTypes.h"
#include "Net/UnrealNetwork.h"


// Sets default values
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
	
}

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

void AUnit::SetGrid(AGrid* NewGrid)
{
	Grid = NewGrid;
}

void AUnit::Destroyed()
{
	Grid->GridInfo->RemoveUnitInGrid(this);
	Super::Destroyed();
}

// Called when the game starts or when spawned
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
		GetWorld()->GetTimerManager().SetTimer(timerTestOnTower, this, &AUnit::testOnTower, 2.0f, false);
	}
	else 
    {
		SetGrid();
    }
	GhostsMesh->SetStaticMesh(UnitMesh->GetStaticMesh());
	GhostsFinaleLocationMesh->SetStaticMesh(UnitMesh->GetStaticMesh());


}

void AUnit::SetGrid()
{
	//get the grid
	AActor* FoundActors = UGameplayStatics::GetActorOfClass(GetWorld(), AGrid::StaticClass());
	if (FoundActors != nullptr)
	{
		Grid = Cast<AGrid>(FoundActors);
		Grid->GridInfo->AddUnitInGrid(Grid->ConvertLocationToIndex(GetActorLocation()), this);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Grid Found"));
		//delay 0.2s and try again
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AUnit::SetGrid, 0.2f, false);
		
	}
}

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

void AUnit::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
	SetIsSelected(true);
}

void AUnit::InitGhosts_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Init Ghosts"));
	GhostsMesh->SetVisibility(true);
	GhostsMesh->SetWorldLocation(GetActorLocation());
	GhostsFinaleLocationMesh->SetWorldLocation(Grid->ConvertIndexToLocation(FutureMovementPos));
	GhostsFinaleLocationMesh->SetVisibility(true);
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
	if(PathToFollowGhost.Num() == 0)
	{
		return;
	}
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

// Called every frame
void AUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(HasAuthority())
		MoveGhosts(DeltaTime);
}

// Called to bind functionality to input
void AUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AUnit::Server_AddOnGrid_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Unit Add on grid"));
	Grid->GridInfo->AddUnitInGrid(Grid->ConvertLocationToIndex(GetActorLocation()), this);
}

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

void AUnit::MoveUnitEndTurn()
{
	Multi_HiddeGhosts();
	InitializeFullMove(FutureMovement);
}

void AUnit::Multi_HiddeGhosts_Implementation()
{
	GhostsMesh->SetVisibility(false);
	GhostsFinaleLocationMesh->SetVisibility(false);
	bIsGhosts = false;
}

void AUnit::TakeDamage(int Damage)
{
GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("UNIT TAKE DAMAGE"));
	if(bBuffTank && (Damage-(Defense+1)) > 0)
	{
		CurrentHealth -= (Damage-(Defense+1));
	}
	else if((Damage-Defense) > 0)
		CurrentHealth -= (Damage-Defense);
}

void AUnit::AttackUnit()
{
	AUnit* UnitToAttack = UnitToAttackRef; 
	if(UnitToAttack == nullptr || Grid == nullptr || UnitToAttack == this)
	{
		return;
	}
	UnitToAttack->TakeDamage(GetAttack());

	TakeDamage(UnitToAttack->GetAttack());

	if(UnitToAttack->GetCurrentHealth() < 1)
	{
		FutureMovement.Insert(UnitToAttack->GetIndexPosition(), 0);
		Grid->GridInfo->RemoveUnitInGrid(UnitToAttack);
		PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->SetUnits(PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->GetUnits() - 1);
		if (UnitToAttack->BuildingRef)
		{
			UnitToAttack->BuildingRef->UnitRef = nullptr;
			UnitToAttack->BuildingRef->GarrisonFull = false;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Unit Got Killed and removed")));
		}
		UnitToAttack->Destroyed();
		if(GetCurrentHealth() < 1)
		{
			Grid->GridInfo->RemoveUnitInGrid(this);
			Grid->GridVisual->RemoveStateFromTile(IndexPosition, EDC_TileState::Selected);
			PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->SetUnits(PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->GetUnits() - 1);
			Destroyed();
		}
		else
			Move(FutureMovement);
	}
	if(GetCurrentHealth() < 1)
	{
		Grid->GridInfo->RemoveUnitInGrid(this);
		Grid->GridVisual->RemoveStateFromTile(IndexPosition, EDC_TileState::Selected);
		PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->SetUnits(PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->GetUnits() - 1);
		Destroyed();
	}


}

void AUnit::AttackBase(ABase* BaseToAttack)
{
	if(BaseToAttack == nullptr || Grid == nullptr)
	{
		return;
	}
	BaseToAttack->TakeDamage(/*GetAttack()*/100);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("health Base: ") + FString::FromInt(BaseToAttack->GetHealth()));
}

void AUnit::Special()
{
	
}

bool AUnit::GetIsGarrison()
{
	return IsGarrison;
}

void AUnit::SetIsGarrison(bool bG)
{
	IsGarrison = bG;
}

ABuilding* AUnit::GetBuildingRef()
{
	return BuildingRef;
}

void AUnit::SetBuildingRef(ABuilding* Building)
{
	BuildingRef = Building;
}

void AUnit::SpecialUnit(AUnit* UnitToAttack)
{
}

UStaticMeshComponent* AUnit::GetFinalGhostMesh()
{
	return GhostsFinaleLocationMesh;
}

void AUnit::SpecialBase(ABase* BaseToAttack)
{
}

void AUnit::NewTurn()
{
	SetIsSelected(false);
	bIsClimbing = false;
	SetBuffTank(false);
	
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
		SetActorLocation(Grid->ConvertIndexToLocation(PathToCross[PathToCrossPosition]) + FVector(0,0,50));
		Grid->GridVisual->RemoveStateFromTile(PathToCross[PathToCrossPosition], EDC_TileState::Pathfinding);

		// If you cross a building
		if (Grid->GetGridData()->Find(PathToCross[PathToCrossPosition])->BuildingOnTile)
		{
			// If the building is empty
			if (Grid->GetGridData()->Find(PathToCross[PathToCrossPosition])->BuildingOnTile->GarrisonFull != true)
			{
				SetActorLocation(Grid->GetGridData()->Find(PathToCross[PathToCrossPosition])->BuildingOnTile->GetActorLocation());
				Grid->GetGridData()->Find(PathToCross[PathToCrossPosition])->BuildingOnTile->UnitRef = this;
				Grid->GetGridData()->Find(PathToCross[PathToCrossPosition])->BuildingOnTile->GarrisonFull = true;

				Grid->GridVisual->RemoveStateFromTile(PathToCross[PathToCrossPosition], EDC_TileState::Pathfinding);
				SetIsGarrison(true);
				bJustBecameGarrison = true;
				BuildingRef = Grid->GetGridData()->Find(PathToCross[PathToCrossPosition])->BuildingOnTile;
				if (PlayerControllerRef->PlayerStateRef != nullptr)
				{
					BuildingRef->SwitchOwner(PlayerControllerRef->PlayerStateRef);
				}
			}
			// If the building is full
			else
			{
				if (this != Grid->GetGridData()->Find(PathToCross[PathToCrossPosition])->BuildingOnTile->UnitRef)
				{
					for(FIntPoint Superindex : Path)
					{
						Grid->GridVisual->RemoveStateFromTile(Superindex, EDC_TileState::Pathfinding);
					}
					Grid->GridVisual->RemoveStateFromTile(PathToCross[PathToCrossPosition], EDC_TileState::Pathfinding);
				}
			}
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

		// If is last move
		if (PathToCross[PathToCrossPosition] == PathToCross.Last())
		{
			Grid->GridVisual->RemoveStateFromTile(PathToCross[PathToCrossPosition], EDC_TileState::Pathfinding);
			Grid->GridInfo->Multi_setUnitIndexOnGrid(PathToCross[PathToCrossPosition],this);
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

int AUnit::GetAttack()
{
	return Attack;
}

int AUnit::GetDefense()
{
	return Defense;
}

int AUnit::GetCurrentHealth()
{
	return CurrentHealth;
}

int AUnit::GetMaxHealth()
{
	return MaxHealth;
}

int AUnit::GetMovementCost()
{
	return MovementCost;
}

int AUnit::GetAttackCost()
{
	return AttackCost;
}

int AUnit::GetSpellCost()
{
	return SpellCost;
}

bool AUnit::GetIsSelected()
{
	return IsSelected;
}

bool AUnit::GetIsHell()
{
	return IsHell;
}

EPlayer AUnit::GetUnitTeam()
{
	return PlayerOwner;
}

void AUnit::SetAttack(int a)
{
	Attack = a;
}

void AUnit::SetDefense(int d)
{
	Defense = d;
}

void AUnit::SetCurrentHealth(int ch)
{
	CurrentHealth = ch;
}

void AUnit::SetMaxHealth(int mh)
{
	MaxHealth = mh;
}

void AUnit::SetMovementCost(int mc)
{
	MovementCost = mc;
}

void AUnit::SetAttackCost(int ac)
{
	AttackCost = ac;
}

void AUnit::SetSpellCost(int sc)
{
	SpellCost = sc;
}

void AUnit::SetIsSelected(bool s)
{
	IsSelected = s;
}

void AUnit::SetIsHell(bool h)
{
	IsHell = h;
}

void AUnit::SetUnitTeam(EPlayer PO)
{
	PlayerOwner = PO;
}

int AUnit::GetTotalDamageInflicted()
{
	return TotalDamagesInflicted;
}

FString AUnit::GetName()
{
	return Name;
}

int AUnit::GetPM()
{
	return PM;
}


FIntPoint AUnit::GetIndexPosition()
{
	return IndexPosition;
}

EPlayer AUnit::GetPlayerOwner()
{
	return PlayerOwner;
}

bool AUnit::GetIsClimbing()
{
	return bIsClimbing;
}

bool AUnit::GetBuffTank()
{
	return bBuffTank;
}

void AUnit::SetTotalDamageInflicted(int tdi)
{
	TotalDamagesInflicted = tdi;
}

void AUnit::SetName(FString n)
{
	Name = n;
}

void AUnit::SetPM(int p)
{
	PM = p;
}


void AUnit::SetIndexPosition(FIntPoint ip)
{
	IndexPosition = ip;
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

void AUnit::SetIsClimbing(bool ic)
{
	bIsClimbing = ic;
}

void AUnit::SetBuffTank(bool bt)
{
	bBuffTank = bt;
}

void AUnit::Multi_PrepareMove_Implementation(const TArray<FIntPoint>& NewPos)
{
	FutureMovement = NewPos;
	FutureMovementPos = FutureMovement.Last();
	InitGhosts();
	Grid->GridInfo->Server_setUnitIndexOnGrid(IndexPosition,this);
	//PlayerControllerRef->AllPlayerActions.Add(FStructActions(this, EDC_ActionPlayer::MoveUnit));
}


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
		if (BuildingToAttackRef->UnitRef && (BuildingToAttackRef->UnitRef->GetUnitTeam() != PlayerControllerRef->GetPlayerTeam()))
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

void AUnit::PrepareSpecial(FIntPoint SpecialPos)
{
	if (Grid->GetGridData()->Find(SpecialPos)->UnitOnTile && PlayerControllerRef->CurrentPA > 1)
	{
		UsedSpecial = true;
		PlayerControllerRef->CurrentPA--;
	}
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

}
