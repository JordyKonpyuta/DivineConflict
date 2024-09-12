// Fill out your copyright notice in the Description page of Project Settings.


#include "GridInfo.h"
#include "Grid.h"
#include "Spawner.h"
#include "F_DC_TileData.h"
#include "Unit.h"

// Sets default values for this component's properties
UGridInfo::UGridInfo()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UGridInfo::AddUnitInGrid(FVector2D GridPosition, AUnit* Unit)
{
	UnitsCombat.Add(Unit);
	setUnitIndexOnGrid(GridPosition, Unit);
}

void UGridInfo::setUnitIndexOnGrid(FVector2D GridPosition, AUnit* Unit)
{
	if(GridPosition != Unit->GetIndexPosition())
	{
		FDC_TileData* PerviousIndex = Grid->GetGridData().Find(Unit->GetIndexPosition());

		if(PerviousIndex != nullptr)
		{
			if(PerviousIndex->UnitOnTile == Unit)
			{
				Grid->GetGridData().Add(PerviousIndex->TilePosition, FDC_TileData(PerviousIndex->TilePosition, PerviousIndex->TileType, PerviousIndex->TileTransform, PerviousIndex->TileState, nullptr, PerviousIndex->SpawnerOnTile));
			}
		}
		Unit->SetIndexPosition(GridPosition);
		if(Unit->GetIndexPosition() != FVector2d(-999,-999))
		{
			FDC_TileData* NewIndex = Grid->GetGridData().Find(Unit->GetIndexPosition());
			if(NewIndex != nullptr)
			{
				Grid->GetGridData().Add(NewIndex->TilePosition, FDC_TileData(NewIndex->TilePosition, NewIndex->TileType, NewIndex->TileTransform, NewIndex->TileState, Unit, NewIndex->SpawnerOnTile));
			}
			NewIndex = Grid->GetGridData().Find(Unit->GetIndexPosition());
			//print NewIndex
			if(NewIndex != nullptr)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
					FString::Printf(TEXT("Unit on tile %s and this position is %s"), *NewIndex->UnitOnTile->GetName(), *NewIndex->TilePosition.ToString()));
			}
		}
	}
}

void UGridInfo::RemoveUnitInGrid(AUnit* Unit)
{
	UnitsCombat.Remove(Unit);
	setUnitIndexOnGrid(FVector2d(-999,-999), Unit);
}

void UGridInfo::addSpawnUnitOnGrid(FVector2D GridPosition, ASpawner* Spawner)
{
	SpawnersGrid.Add(Spawner);
	SetSpawnUnitOnGrid(GridPosition, Spawner);
	
}

void UGridInfo::SetSpawnUnitOnGrid(FVector2D GridPosition, ASpawner* Spawner)
{

	if(GridPosition != Spawner->GetGridPosition())
	{
		FDC_TileData* PerviousIndex = Grid->GetGridData().Find(Spawner->GetGridPosition());

		if(PerviousIndex != nullptr)
		{
			if(PerviousIndex->SpawnerOnTile == Spawner)
			{
				Grid->GetGridData().Add(PerviousIndex->TilePosition, FDC_TileData(PerviousIndex->TilePosition, PerviousIndex->TileType, PerviousIndex->TileTransform, PerviousIndex->TileState, PerviousIndex->UnitOnTile, nullptr));
			}
		}
		Spawner->SetGridPosition(GridPosition);
		if(Spawner->GetGridPosition() != FVector2d(-999,-999))
		{
			FDC_TileData* NewIndex = Grid->GetGridData().Find(Spawner->GetGridPosition());
			if(NewIndex != nullptr)
			{
				Grid->GetGridData().Add(NewIndex->TilePosition, FDC_TileData(NewIndex->TilePosition, NewIndex->TileType, NewIndex->TileTransform, NewIndex->TileState, NewIndex->UnitOnTile, Spawner));
			}
			
		}
	}
	
}


// Called when the game starts
void UGridInfo::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGridInfo::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGridInfo::SetGrid(AGrid* GridRef)
{
	Grid = GridRef;
}

