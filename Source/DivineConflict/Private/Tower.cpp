// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"

#include "CustomPlayerController.h"
#include "GridInfo.h"
#include "GridPath.h"
#include "Unit.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATower::ATower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TArray<AActor*> FoundActor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ACustomPlayerController::StaticClass(),FoundActor);
	for(AActor* Actor : FoundActor)
	{
		PlayerController = Cast<ACustomPlayerController>(Actor);
	}

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
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("Grid is not null"));
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

bool ATower::GetIsHell()
{
	return IsHell;
}

void ATower::SetIsHell(bool NewIsHell)
{
	IsHell = NewIsHell;
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

void ATower::AttackUnit(AUnit* UnitToAttack)
{
	UnitToAttack->SetCurrentHealth(UnitToAttack->GetCurrentHealth() - Attack);
	SetCanAttack(false);
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


