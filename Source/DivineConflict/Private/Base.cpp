// Fill out your copyright notice in the Description page of Project Settings.


#include "Base.h"
#include "CustomPlayerController.h"
#include "Grid.h"
#include "GridInfo.h"
#include "CustomPlayerState.h"
#include "GridVisual.h"
#include "Unit.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABase::ABase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ABase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABase, Health);
	DOREPLIFETIME(ABase, GoldCostUpgrade);
	DOREPLIFETIME(ABase, StoneCostUpgrade);
	DOREPLIFETIME(ABase, WoodCostUpgrade);
	DOREPLIFETIME(ABase, GridPosition);
}

// Called when the game starts or when spawned
void ABase::BeginPlay()
{
	Super::BeginPlay();

	SetMesh();

	TArray<AActor*> FoundActor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ACustomPlayerController::StaticClass(),FoundActor);
	for(AActor* Actor : FoundActor)
	{
		PlayerControllerRef = Cast<ACustomPlayerController>(Actor);
	}

	PlayerStateRef = PlayerControllerRef->GetPlayerState<ACustomPlayerState>();

	if (Grid)
	{
		// Create Building Data
	
		// Grid : Building
		Grid->GridInfo->addBaseOnGrid(Grid->ConvertLocationToIndex(GetActorLocation()), this);

		// Grid : Spawnable Locations
		AllSpawnLoc.Add(Grid->ConvertLocationToIndex(GetActorLocation()+ FVector3d(100,0,0)));
		AllSpawnLoc.Add(Grid->ConvertLocationToIndex(GetActorLocation()+ FVector3d(0,100,0)));
		AllSpawnLoc.Add(Grid->ConvertLocationToIndex(GetActorLocation()+ FVector3d(100,-100,0)));
		AllSpawnLoc.Add(Grid->ConvertLocationToIndex(GetActorLocation()+ FVector3d(-100,100,0)));
		
		}
}

// Called every frame
void ABase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABase::VisualSpawn()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Turquoise, TEXT("Update Visual"));
	for (int i = 0; i < AllSpawnLoc.Num(); i++)
	{
		if (IsSelected)
			{
				if (Grid->GetGridData()->Find(AllSpawnLoc[i])->UnitOnTile == nullptr)
				{
						Grid->GridVisual->addStateToTile(AllSpawnLoc[i], EDC_TileState::Spawnable);
						break;
				}
			}
		else
		{
			Grid->GridVisual->RemoveStateFromTile(AllSpawnLoc[i], EDC_TileState::Spawnable);
		}
	}
}

void ABase::SpawnUnit(EUnitType UnitToSpawn)
{
	if (Grid != nullptr)
	{
		FIntPoint SpawnLoc;
		for (int i = 0; i < AllSpawnLoc.Num(); i++)
		{
			if (Grid->GetGridData()->Find(AllSpawnLoc[i])->UnitOnTile == nullptr)
			{
				SpawnLoc = AllSpawnLoc[i];
				break;
			}
		}
		
		 if (PlayerStateRef->CurrentUnitCount < PlayerStateRef->MaxUnitCount)
		{
			if (PlayerStateRef->GetGoldPoints() >= 0 && PlayerStateRef->GetStonePoints() >= 0 && PlayerStateRef->GetWoodPoints() >= 0)
			{
				PlayerStateRef->ChangeGoldPoints(0, false);
				PlayerStateRef->ChangeStonePoints(0, false);
				PlayerStateRef->ChangeWoodPoints(0, false);
				PlayerStateRef->SetUnits(PlayerStateRef->CurrentUnitCount + 1);
				PlayerControllerRef->SpawnUnit(UnitToSpawn, SpawnLoc);
				Grid->GridVisual->RemoveStateFromTile(SpawnLoc, EDC_TileState::Spawnable);
			}
			else
			{
				Grid->GridVisual->RemoveStateFromTile(SpawnLoc, EDC_TileState::Spawnable);
			}
		}
		else
		{
			Grid->GridVisual->RemoveStateFromTile(SpawnLoc, EDC_TileState::Spawnable);
		}
	}
}

int ABase::GetHealth()
{
	return Health;
}

int ABase::GetGoldCostUpgrade()
{
	return GoldCostUpgrade;
}

int ABase::GetStoneCostUpgrade()
{
	return StoneCostUpgrade;
}

int ABase::GetWoodCostUpgrade()
{
	return WoodCostUpgrade;
}

FIntPoint ABase::GetGridPosition()
{
	return GridPosition;
}

void ABase::SetHealth(int h)
{
	Health = h;
}

void ABase::SetCostsUpgrade(int g, int s, int w)
{
	GoldCostUpgrade = g;
	StoneCostUpgrade = s;
	WoodCostUpgrade = w;
}

void ABase::SetGridPosition(FIntPoint GridP)
{
	GridPosition = GridP;
}

// IF DEAD, END GAME
void ABase::CheckIfDead()
{
	if (Health <= 0)
	{
		Health = 0;
		PlayerControllerRef->SetIsDead(true);
		OnDeath();
		
	}
}

void ABase::SetMesh_Implementation()
{
}


// TAKE DAMAGE
void ABase::TakeDamage(int Damage)
{
	Health -= Damage;
	CheckIfDead();
}

// UPGRADE
void ABase::Upgrade()
{
	if (PlayerStateRef->GetWoodPoints() >= WoodCostUpgrade && PlayerStateRef->GetStonePoints() >= StoneCostUpgrade && PlayerStateRef->GetGoldPoints() >= GoldCostUpgrade)
	{
		if (PlayerStateRef->MaxUnitCount < 15)
		{
			PlayerStateRef->MaxUnitCount += 5;
			SetCostsUpgrade(GoldCostUpgrade += 10, StoneCostUpgrade += 10, WoodCostUpgrade += 10);
		}
	}
}



// FUNCTION FOR UI
void ABase::OnDeath_Implementation()
{
}

