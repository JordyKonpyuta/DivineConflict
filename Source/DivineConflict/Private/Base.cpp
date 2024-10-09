// Fill out your copyright notice in the Description page of Project Settings.


#include "Base.h"

#include "CustomGameState.h"
#include "CustomPlayerController.h"
#include "Grid.h"
#include "GridInfo.h"
#include "CustomPlayerState.h"
#include "GridVisual.h"
#include "SNegativeActionButton.h"
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
	DOREPLIFETIME(ABase, AllSpawnLoc);
}

// Called when the game starts or when spawned
void ABase::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < GetWorld()->GetGameState<ACustomGameState>()->PlayerArray.Num(); i++)
	{
		if (GetWorld()->GetGameState<ACustomGameState>()->PlayerArray[i]->GetPlayerController()->GetPlayerState<ACustomPlayerState>()->PlayerTeam == PlayerOwner)
		{
			PlayerStateRef = GetWorld()->GetGameState<ACustomGameState>()->PlayerArray[i]->GetPlayerController()->GetPlayerState<ACustomPlayerState>();
		}
	}

	SetMesh();
	// Get Grid
	if (Grid)
	{
	
		// Grid : Building
		Grid->GridInfo->addBaseOnGrid(Grid->ConvertLocationToIndex(GetActorLocation()), this);

		// Grid : Spawnable Locations
		int Ratio = 0;

		if (PlayerOwner == EPlayer::P_Heaven) Ratio = -1;
		else if (PlayerOwner == EPlayer::P_Hell) Ratio = 1;
			
		AllSpawnLoc.Add(GetGridPosition() + FIntPoint(1*Ratio, 0));
		AllSpawnLoc.Add(GetGridPosition() + FIntPoint(0, 1*Ratio));
		AllSpawnLoc.Add(GetGridPosition() + FIntPoint(1*Ratio, -1*Ratio));
		AllSpawnLoc.Add(GetGridPosition() + FIntPoint(-1*Ratio, 1*Ratio));
	}
	
}

// Called every frame
void ABase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABase::VisualSpawn()
{
	AllSpawnLoc.Empty();
	int Ratio = 0;

	if (PlayerOwner == EPlayer::P_Heaven) Ratio = -1;
	else if (PlayerOwner == EPlayer::P_Hell) Ratio = 1;
			
	AllSpawnLoc.Add(GetGridPosition() + FIntPoint(1*Ratio, 0));
	AllSpawnLoc.Add(GetGridPosition() + FIntPoint(0, 1*Ratio));
	AllSpawnLoc.Add(GetGridPosition() + FIntPoint(1*Ratio, -1*Ratio));
	AllSpawnLoc.Add(GetGridPosition() + FIntPoint(-1*Ratio, 1*Ratio));
	
	for (FIntPoint i : AllSpawnLoc)
	{
		UE_LOG( LogTemp, Warning, TEXT("i : %s"), *i.ToString());
		if (IsSelected)
			{
			UE_LOG( LogTemp, Warning, TEXT("i : %s"), *i.ToString());
				if (Grid->GetGridData()->Find(i)->UnitOnTile == nullptr)
				{
					UE_LOG( LogTemp, Warning, TEXT("i : %s"), *i.ToString());
					Grid->GridVisual->addStateToTile(i, EDC_TileState::Spawnable);
					return;
				}
			}
		else
		{
			Grid->GridVisual->RemoveStateFromTile(i, EDC_TileState::Spawnable);
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

void ABase::ServerCheckIfDead_Implementation()
{
	MulticastCheckIfDead();
}

// IF DEAD, END GAME
void ABase::MulticastCheckIfDead_Implementation()
{
	if (Health <= 0)
	{
		//OnDeath();
		GetWorld()->GetGameState<ACustomGameState>()->ServerVictoryScreen(PlayerOwner);
	}	
}

void ABase::SetMesh_Implementation()
{
}


// TAKE DAMAGE
void ABase::TakeDamage(int Damage)
{
	Health -= Damage;
	ServerCheckIfDead();
}

// UPGRADE MaxUnitCount
void ABase::Upgrade()
{
	if(PlayerStateRef)
		// Check if player has enough resources
		if (PlayerStateRef->GetWoodPoints() >= WoodCostUpgrade && PlayerStateRef->GetStonePoints() >= StoneCostUpgrade && PlayerStateRef->GetGoldPoints() >= GoldCostUpgrade)
		{
			if (PlayerStateRef->MaxUnitCount < 15)
			{
				PlayerStateRef->MaxUnitCount += 5;
				PlayerStateRef->ChangeWoodPoints(WoodCostUpgrade, false);
				PlayerStateRef->ChangeStonePoints(StoneCostUpgrade, false);
				PlayerStateRef->ChangeGoldPoints(GoldCostUpgrade, false);
				SetCostsUpgrade(GoldCostUpgrade += 10, StoneCostUpgrade += 10, WoodCostUpgrade += 10);
			}
		}

	else UE_LOG( LogTemp, Warning, TEXT("Player State Ref is NULL"));
}


// FUNCTION FOR UI
void ABase::OnDeath_Implementation()
{
}

