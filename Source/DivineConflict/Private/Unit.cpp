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

		Grid->GridInfo->AddUnitInGrid(Grid->ConvertLocationToIndex(GetActorLocation()), this);

		
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
	else 
    {
		SetGrid();
    }
	
}

void AUnit::SetGrid()
{
	//get the grid
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Set Grid"));
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

void AUnit::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
	SetIsSelected(true);
}

// Called every frame
void AUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

void AUnit::Move_Implementation(const TArray<FIntPoint>& PathIn)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Unit moved"));
	bool bJustBecameGarrison = false;
	Path.Empty();
	Path = PathIn;
	if (Path.Num()!=-1)
	{
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
}


void AUnit::AttackUnit(AUnit* UnitToAttack)
{
	if(UnitToAttack == nullptr || Grid == nullptr || UnitToAttack == this)
	{
		return;
	}
	if (UnitToAttack->GetBuffTank())
	{
		if(UnitToAttack->GetAttack()-(GetDefense()+1)  > 0)
			SetCurrentHealth(GetCurrentHealth()-( UnitToAttack->GetAttack()-(GetDefense()+1 )));
	}
	else
	{
		if(UnitToAttack->GetAttack()- GetDefense() > 0)
			SetCurrentHealth( GetCurrentHealth() - (UnitToAttack->GetAttack()- GetDefense()));
	}
	if(GetBuffTank())
	{
		if(GetAttack()- UnitToAttack->GetDefense() > 0)
			UnitToAttack->SetCurrentHealth( UnitToAttack->GetCurrentHealth() - (GetAttack()- UnitToAttack->GetDefense()));
	}
	else 
	{
		if(GetAttack()- UnitToAttack->GetDefense() > 0)
			UnitToAttack->SetCurrentHealth( UnitToAttack->GetCurrentHealth() - (GetAttack()- UnitToAttack->GetDefense()));
	}

	

	if(UnitToAttack->GetCurrentHealth() < 1)
	{
		Path.Add(UnitToAttack->GetIndexPosition());
		Grid->GridInfo->RemoveUnitInGrid(UnitToAttack);
		PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->SetUnits(PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->GetUnits() - 1);
		if (UnitToAttack->BuildingRef)
		{
			UnitToAttack->BuildingRef->UnitRef = nullptr;
			UnitToAttack->BuildingRef->GarrisonFull = false;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Unit Got Killed and removed")));
		}
		GetWorld()->DestroyActor(UnitToAttack);
		if(GetCurrentHealth() < 1)
		{
			Grid->GridInfo->RemoveUnitInGrid(this);
			Grid->GridVisual->RemoveStateFromTile(IndexPosition, EDC_TileState::Selected);
			PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->SetUnits(PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->GetUnits() - 1);
			GetWorld()->DestroyActor(this);
		}
		else
		Move(Path);
	}
	if(GetCurrentHealth() < 1)
	{
		Grid->GridInfo->RemoveUnitInGrid(this);
		Grid->GridVisual->RemoveStateFromTile(IndexPosition, EDC_TileState::Selected);
		PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->SetUnits(PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->GetUnits() - 1);
		GetWorld()->DestroyActor(this);
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

void AUnit::SpecialBase(ABase* BaseToAttack)
{
}

void AUnit::NewTurn()
{
	SetIsSelected(false);
	bIsClimbing = false;
	SetBuffTank(false);
	
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
}

void AUnit::SetIsClimbing(bool ic)
{
	bIsClimbing = ic;
}

void AUnit::SetBuffTank(bool bt)
{
	bBuffTank = bt;
}


