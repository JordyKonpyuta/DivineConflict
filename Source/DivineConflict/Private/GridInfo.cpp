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

void UGridInfo::AddUnitInGrid(FIntPoint GridPosition, AUnit* Unit)
{
	UnitsCombat.Add(Unit);
	setUnitIndexOnGrid(GridPosition, Unit);
}

void UGridInfo::setUnitIndexOnGrid(FIntPoint GridPosition, AUnit* Unit)
{
	if(Grid == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("Grid is null"));
		return;
	}
	TMap<FIntPoint,FDC_TileData> GridRef = Grid->GetGridData();
	
	if(GridPosition != Unit->GetIndexPosition())
	{
		FDC_TileData* PerviousIndex = GridRef.Find(Unit->GetIndexPosition());

		if(PerviousIndex != nullptr)
		{
			if(PerviousIndex->UnitOnTile == Unit)
			{
				GridRef.Add(PerviousIndex->TilePosition, FDC_TileData(PerviousIndex->TilePosition, PerviousIndex->TileType, PerviousIndex->TileTransform, PerviousIndex->TileState, nullptr, PerviousIndex->SpawnerOnTile));
				Grid->SetGridData(GridRef);
			}
		}
		Unit->SetIndexPosition(GridPosition);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Unit Position : " + FString::FromInt(Unit->GetIndexPosition().X) + " " + FString::FromInt(Unit->GetIndexPosition().Y)));
		if(Unit->GetIndexPosition() != FIntPoint(-999,-999))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Grid Position : "  + FString::FromInt(GridPosition.X) + " " + FString::FromInt(GridPosition.Y)));
			FDC_TileData* NewIndex = GridRef.Find(GridPosition);
			GEngine->AddOnScreenDebugMessage(-1 , 5.f, FColor::Green, TEXT("Tile Location : " + NewIndex->TileTransform.GetLocation().ToString()));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Tile Position : " + FString::FromInt(NewIndex->TilePosition.X) + " " + FString::FromInt(NewIndex->TilePosition.Y)));
			if(NewIndex != nullptr)
			{
				TMap<FIntPoint,FDC_TileData> GridTemp = Grid->GetGridData();
				GridTemp.Add(NewIndex->TilePosition, FDC_TileData(NewIndex->TilePosition, NewIndex->TileType, NewIndex->TileTransform, NewIndex->TileState, Unit, NewIndex->SpawnerOnTile));
				Grid->SetGridData(GridTemp);
				
				
			}
			NewIndex = Grid->GetGridData().Find(Unit->GetIndexPosition());
			//print NewIndex
			if(NewIndex != nullptr)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Unit added to grid"));
			}
		}
	}
}

void UGridInfo::RemoveUnitInGrid(AUnit* Unit)
{
	UnitsCombat.Remove(Unit);
	setUnitIndexOnGrid(FIntPoint(-999,-999), Unit);
}

void UGridInfo::addSpawnUnitOnGrid(FIntPoint GridPosition, ASpawner* Spawner)
{
	SpawnersGrid.Add(Spawner);
	SetSpawnUnitOnGrid(GridPosition, Spawner);
	
}

void UGridInfo::SetSpawnUnitOnGrid(FIntPoint GridPosition, ASpawner* Spawner)
{
	if(Grid == nullptr)
	{
		return;
	}

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
		if(Spawner->GetGridPosition() != FIntPoint(-999,-999))
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

