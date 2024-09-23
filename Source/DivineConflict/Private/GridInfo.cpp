// Fill out your copyright notice in the Description page of Project Settings.


#include "GridInfo.h"
#include "Base.h"
#include "Building.h"
#include "Grid.h"
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
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("Grid is null"));
		return;
	}
	TMap<FIntPoint,FDC_TileData>* GridDataRef = Grid->GetGridData();
	if(GridPosition != Unit->GetIndexPosition())
	{
		FDC_TileData* PerviousIndex = GridDataRef->Find(Unit->GetIndexPosition());

		if(PerviousIndex != nullptr)
		{
			if(PerviousIndex->UnitOnTile == Unit)
			{
				GridDataRef->Add(PerviousIndex->TilePosition, FDC_TileData(PerviousIndex->TilePosition, PerviousIndex->TileType, PerviousIndex->TileTransform, PerviousIndex->TileState, nullptr, PerviousIndex->SpawnerOnTile , PerviousIndex->BaseOnTile));

			}
		}
		Unit->SetIndexPosition(GridPosition);

		if(Unit->GetIndexPosition() != FIntPoint(-999,-999))
		{

			if(GridDataRef->Find(FIntPoint(0,0)) != nullptr)
			{
				GridDataRef->Add(GridDataRef->Find(GridPosition)->TilePosition, FDC_TileData(GridDataRef->Find(GridPosition)->TilePosition, GridDataRef->Find(GridPosition)->TileType,
					GridDataRef->Find(GridPosition)->TileTransform, GridDataRef->Find(GridPosition)->TileState, Unit, GridDataRef->Find(GridPosition)->SpawnerOnTile, GridDataRef->Find(GridPosition)->BaseOnTile));
			}
			else
			{
			}
		}
	}
}

void UGridInfo::RemoveUnitInGrid(AUnit* Unit)
{
	UnitsCombat.Remove(Unit);
	setUnitIndexOnGrid(FIntPoint(-999,-999), Unit);
}

void UGridInfo::addSpawnUnitOnGrid(FIntPoint GridPosition, ABuilding* Spawner)
{
	SpawnersGrid.Add(Spawner);
	SetSpawnUnitOnGrid(GridPosition, Spawner);
	
}

void UGridInfo::SetSpawnUnitOnGrid(FIntPoint GridPosition, ABuilding* Spawner)
{
	if(Grid == nullptr)
	{
		return;
	}

	if(GridPosition != Spawner->GetGridPosition())
	{
		FDC_TileData* PerviousIndex = Grid->GetGridData()->Find(Spawner->GetGridPosition());

		if(PerviousIndex != nullptr)
		{
			if(PerviousIndex->SpawnerOnTile == Spawner)
			{
				Grid->GetGridData()->Add(PerviousIndex->TilePosition, FDC_TileData(PerviousIndex->TilePosition, PerviousIndex->TileType, PerviousIndex->TileTransform, PerviousIndex->TileState, PerviousIndex->UnitOnTile, nullptr, PerviousIndex->BaseOnTile));
			}
		}
		Spawner->SetGridPosition(GridPosition);
		if(Spawner->GetGridPosition() != FIntPoint(-999,-999))
		{
			FDC_TileData* NewIndex = Grid->GetGridData()->Find(Spawner->GetGridPosition());
			if(NewIndex != nullptr)
			{
				Grid->GetGridData()->Add(NewIndex->TilePosition, FDC_TileData(NewIndex->TilePosition, NewIndex->TileType, NewIndex->TileTransform, NewIndex->TileState, NewIndex->UnitOnTile, Spawner, NewIndex->BaseOnTile));
			}
			
		}
	}
	
}

void UGridInfo::addBaseOnGrid(FIntPoint GridPosition, ABase* Base)
{
	BasesGrid.Add(Cast<ABase>(Base));
	SetBaseOnGrid(GridPosition, Base);
}

void UGridInfo::SetBaseOnGrid(FIntPoint GridPosition, ABase* Base)
{
	if(Grid == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("Grid is null"));
		return;
	}
	if(GridPosition != Base->GetGridPosition())
	{
		FDC_TileData* PerviousIndex = Grid->GetGridData()->Find(Base->GetGridPosition());

		if(PerviousIndex != nullptr)
		{
			if(PerviousIndex->BaseOnTile == Base)
			{
				Grid->GetGridData()->Add(PerviousIndex->TilePosition, FDC_TileData(PerviousIndex->TilePosition, PerviousIndex->TileType, PerviousIndex->TileTransform, PerviousIndex->TileState, PerviousIndex->UnitOnTile, PerviousIndex->SpawnerOnTile, nullptr));
			}
		}
		Base->SetGridPosition(GridPosition);
		if(Base->GetGridPosition() != FIntPoint(-999,-999))
		{
			FDC_TileData* NewIndex = Grid->GetGridData()->Find(Base->GetGridPosition());
			if(NewIndex != nullptr)
			{
				Grid->GetGridData()->Add(NewIndex->TilePosition, FDC_TileData(NewIndex->TilePosition, NewIndex->TileType, NewIndex->TileTransform, NewIndex->TileState, NewIndex->UnitOnTile, NewIndex->SpawnerOnTile, Base));
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
	UE_LOG( LogTemp, Warning, TEXT("Grid Data : %d "),Grid->GridData.Num());
}

