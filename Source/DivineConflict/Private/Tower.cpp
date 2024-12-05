// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"

#include "CustomPlayerController.h"
#include "GridInfo.h"
#include "GridPath.h"
#include "GridVisual.h"
#include "IMovieSceneTracksModule.h"
#include "Projectile.h"
#include "Unit.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


	// ----------------------------
	// Constructor
ATower::ATower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
}

	// ----------------------------
	// Overrides

void ATower::BeginPlay()
{
	Super::BeginPlay();

	//SetMesh();

	if(Grid)
	{
		Grid->GridInfo->AddTowerOnGrid(Grid->ConvertLocationToIndex(GetActorLocation()), this);
		// Set Reachable Tiles for Attack
		TilesInRange = Grid->GridPath->FindPath(GridPosition,FIntPoint(-999,-999),true,4,false,false);
	}

}

void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

	// ----------------------------
	// REPLICATIONS !!!

void ATower::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATower, Attack);
	DOREPLIFETIME(ATower, bCanAttack);
	DOREPLIFETIME(ATower, GridPosition);
	DOREPLIFETIME(ATower, Mesh);
	DOREPLIFETIME(ATower, PlayerOwner);
	DOREPLIFETIME(ATower, UnitToAttack);
	DOREPLIFETIME(ATower, UnitToAttackPosition);
}

	// ----------------------------
	// Components

void ATower::SetMesh_Implementation()
{
}

	// ----------------------------
	// Attack

void ATower::PrepareAttack(AUnit* UnitAttack)
{
	UnitToAttack = UnitAttack;
	UnitToAttackPosition = UnitAttack->GetIndexPosition();
	bIsSelected = false;
	UpdateVisuals();
}

void ATower::AttackUnit(AUnit* UnitToAttacking, ACustomPlayerController* PlayerControllerAttacking)
{
	Attack= 5;
	bCanAttack = true;
	if (bCanAttack)
	{
		Projectile = GetWorld()->SpawnActor<AProjectile>(AProjectile::StaticClass(), GetActorLocation(), GetActorRotation());
		Projectile->TowerOwner = this;
		Projectile->MoveProjectile(UnitToAttacking);
		
		UnitToAttacking->TakeDamage(UnitToAttacking->GetDefense() + Attack, FDamageEvent(), PlayerControllerAttacking, this);
	}
}

	// ----------------------------
	// Visuals

void ATower::UpdateVisuals()
{
	if (bIsSelected)
	{
		PlayerController->SetPathReachable(TilesInRange);
		for (FIntPoint Tile : TilesInRange)
		{
			// Highlight Tiles
			Grid->GridVisual->AddStateToTile(Tile, EDC_TileState::Attacked);
		}
	}
	else
	{
		for (FIntPoint Tile : TilesInRange)
		{
			// Remove Highlight
			Grid->GridVisual->RemoveStateFromTile(Tile, EDC_TileState::Attacked);
		}
	}
}
	
// ----------------------------
// Tower Upgrade

void ATower::UpgradeTower()
{
	if (	PlayerController->PlayerStateRef->GetWoodPoints() >= WoodUpgradePrice
		&&	PlayerController->PlayerStateRef->GetStonePoints() >= StoneUpgradePrice
		&&	PlayerController->PlayerStateRef->GetGoldPoints() >= GoldUpgradePrice
		&& Level < MaxLevel)
	{
		PlayerController->PlayerStateRef->ChangeWoodPoints(WoodUpgradePrice, false);
		PlayerController->PlayerStateRef->ChangeStonePoints(StoneUpgradePrice, false);
		PlayerController->PlayerStateRef->ChangeGoldPoints(GoldUpgradePrice, false);

		Level++;
		WoodUpgradePrice += 10 + Level * 5;
		StoneUpgradePrice += 10 + Level * 5;
		GoldUpgradePrice += 10 + Level * 5;
		
		switch(Level)
		{
		case 1:
			bCanAttack = true;
			SetAttack(2);
			break;
		case 2:
			SetAttack(3);
			break;
		case 3:
			SetAttack(5);
			break;
		default:
			SetAttack(5);
			break;
		}
	}
}

// ----------------------------
	// GETTERS

int ATower::GetAttack()
{
	return Attack;
}

EPlayer ATower::GetPlayerOwner()
{
	return PlayerOwner;
}

bool ATower::GetCanAttack()
{
	return bCanAttack;
}

FIntPoint ATower::GetGridPosition()
{
	return GridPosition;
}

	// ----------------------------
	// SETTERS

void ATower::SetAttack(int NewAttack)
{
	Attack = NewAttack;
}

void ATower::SetPlayerOwner(EPlayer NewPlayerOwner)
{
	PlayerOwner = NewPlayerOwner;
}

void ATower::SetCanAttack(bool NewCanAttack)
{
	bCanAttack = NewCanAttack;
}

void ATower::SetGridPosition(FIntPoint NewGridPosition)
{
	GridPosition = NewGridPosition;
}