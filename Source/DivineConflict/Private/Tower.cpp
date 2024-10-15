// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"

#include "CustomPlayerController.h"
#include "GridInfo.h"
#include "GridPath.h"
#include "Unit.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATower::ATower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
}

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

void ATower::SetMesh_Implementation()
{
}

// Called when the game starts or when spawned
void ATower::BeginPlay()
{
	Super::BeginPlay();

	//SetMesh();

	if(Grid)
	{
		Grid->GridInfo->addTowerOnGrid(Grid->ConvertLocationToIndex(GetActorLocation()), this);
		// Set Reachable Tiles for Attack
		TilesInRange = Grid->GridPath->FindPath(GridPosition,FIntPoint(-999,-999),true,4,false);
	}

}

// Called every frame
void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int ATower::GetAttack()
{
	return Attack;
}

void ATower::SetAttack(int NewAttack)
{
	Attack = NewAttack;
}


EPlayer ATower::GetPlayerOwner()
{
	return PlayerOwner;
}

void ATower::SetPlayerOwner(EPlayer NewPlayerOwner)
{
	PlayerOwner = NewPlayerOwner;
}

bool ATower::GetCanAttack()
{
	return CanAttack;
}

void ATower::SetCanAttack(bool NewCanAttack)
{
	CanAttack = NewCanAttack;
}

void ATower::AttackUnit(AUnit* UnitToAttacking, ACustomPlayerController* PlayerControllerAttacking)
{

	UnitToAttacking->SetCurrentHealth(UnitToAttacking->GetCurrentHealth() - Attack);
	SetCanAttack(false);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("UnitToAttack : " + FString::FromInt(UnitToAttacking->GetCurrentHealth())));
	if(UnitToAttacking->GetCurrentHealth() < 1)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("UnitToAttack Destroyed"));
		UnitToAttacking->Server_DestroyUnit();
		UnitToAttacking = nullptr;
	}
	
}


void ATower::UpdateVisuals()
{
	if (IsSelected)
	for (FIntPoint Tile : TilesInRange)
	{
			// Highlight Tiles
			Grid->GridVisual->addStateToTile(Tile, EDC_TileState::Attacked);
	} else
	{
		for (FIntPoint Tile : TilesInRange)
		{
			// Remove Highlight
			Grid->GridVisual->RemoveStateFromTile(Tile, EDC_TileState::Attacked);
		}
	}
	
}

FIntPoint ATower::GetGridPosition()
{
	return GridPosition;
}

void ATower::SetGridPosition(FIntPoint NewGridPosition)
{
	GridPosition = NewGridPosition;
}

void ATower::PreprareAttack(AUnit* UnitAttack)
{
	UnitToAttack = UnitAttack;
	UnitToAttackPosition = UnitAttack->GetIndexPosition();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("unitToAttackPosition : " + UnitToAttackPosition.ToString()));
	IsSelected = false;
	UpdateVisuals();
}



