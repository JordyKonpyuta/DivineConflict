// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"

#include "CustomPlayerController.h"
#include "GridInfo.h"
#include "GridPath.h"
#include "Unit.h"

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
	}
		
	/*
	// Set Reachable Tiles for Attack
	TilesInRange.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(100, 0, 0)));
	TilesInRange.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(200, 0, 0)));
	TilesInRange.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(300, 0, 0)));
	TilesInRange.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-100, 0, 0)));
	TilesInRange.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-200, 0, 0)));
	TilesInRange.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-300, 0, 0)));
	TilesInRange.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(0, 100, 0)));
	TilesInRange.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(0, 200, 0)));
	TilesInRange.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(0, 300, 0)));
	TilesInRange.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(0, -100, 0)));
	TilesInRange.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(0, -200, 0)));
	TilesInRange.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(0, -300, 0)));*/
	TilesInRange = Grid->GridPath->FindPath(GridPosition,FIntPoint(-999,-999),true,4,false);
	
	
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

void ATower::AttackUnit(AUnit* UnitToAttack)
{
}

void ATower::UpdateVisuals()
{
	if (IsSelected)
	for (FIntPoint Tile : TilesInRange)
	{
			// Highlight Tiles
			Grid->GridVisual->addStateToTile(Tile, EDC_TileState::Spawnable);
	} else
	{
		for (FIntPoint Tile : TilesInRange)
		{
			// Remove Highlight
			Grid->GridVisual->RemoveStateFromTile(Tile, EDC_TileState::Spawnable);
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


