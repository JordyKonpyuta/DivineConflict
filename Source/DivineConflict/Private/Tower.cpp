// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"

#include "CustomPlayerController.h"
#include "GridInfo.h"
#include "GridPath.h"
#include "IMovieSceneTracksModule.h"
#include "Projectile.h"
#include "Unit.h"
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
		Grid->GridInfo->addTowerOnGrid(Grid->ConvertLocationToIndex(GetActorLocation()), this);
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
	DOREPLIFETIME(ATower, UnitInGarrison);
	DOREPLIFETIME(ATower, IsGarrisoned);
	DOREPLIFETIME(ATower, Attack);
	DOREPLIFETIME(ATower, CanAttack);
	DOREPLIFETIME(ATower, PlayerOwner);
	DOREPLIFETIME(ATower, Mesh);
	DOREPLIFETIME(ATower, GridPosition);
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

void ATower::PreprareAttack(AUnit* UnitAttack)
{
	UnitToAttack = UnitAttack;
	UnitToAttackPosition = UnitAttack->GetIndexPosition();
	IsSelected = false;
	UpdateVisuals();
}

void ATower::AttackUnit(AUnit* UnitToAttacking, ACustomPlayerController* PlayerControllerAttacking)
{
	CannonBall = GetWorld()->SpawnActor<AProjectile>(AProjectile::StaticClass(), GetActorLocation(), GetActorRotation());
	CannonBall->TowerOwner = this;
	CannonBall->MoveProjectile(UnitToAttacking);
	
	UnitToAttacking->SetCurrentHealth(UnitToAttacking->GetCurrentHealth() - Attack);
	if(UnitToAttacking->GetCurrentHealth() < 1)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("UnitToAttack Destroyed"));
		UnitToAttacking->Server_DestroyUnit();
		UnitToAttacking = nullptr;
	}
	
}

	// ----------------------------
	// Visuals

void ATower::UpdateVisuals()
{
	if (IsSelected)
	{
		PlayerController->SetPathReachable(TilesInRange);
		for (FIntPoint Tile : TilesInRange)
		{
			// Highlight Tiles
			Grid->GridVisual->addStateToTile(Tile, EDC_TileState::Attacked);
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
	return CanAttack;
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
	CanAttack = NewCanAttack;
}

void ATower::SetGridPosition(FIntPoint NewGridPosition)
{
	GridPosition = NewGridPosition;
}