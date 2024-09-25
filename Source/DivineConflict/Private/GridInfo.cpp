// Fill out your copyright notice in the Description page of Project Settings.


#include "GridInfo.h"
#include "Base.h"
#include "Building.h"
#include "Grid.h"
#include "F_DC_TileData.h"
#include "Tower.h"
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
				GridDataRef->Add(PerviousIndex->TilePosition, FDC_TileData(PerviousIndex->TilePosition, PerviousIndex->TileType, PerviousIndex->TileTransform, PerviousIndex->TileState, nullptr, PerviousIndex->BuildingOnTile , PerviousIndex->BaseOnTile, PerviousIndex->TowerOnTile));

			}
		}
		Unit->SetIndexPosition(GridPosition);

		if(Unit->GetIndexPosition() != FIntPoint(-999,-999))
		{

			if(GridDataRef->Find(FIntPoint(0,0)) != nullptr)
			{
				GridDataRef->Add(GridDataRef->Find(GridPosition)->TilePosition, FDC_TileData(GridDataRef->Find(GridPosition)->TilePosition, GridDataRef->Find(GridPosition)->TileType,
					GridDataRef->Find(GridPosition)->TileTransform, GridDataRef->Find(GridPosition)->TileState, Unit, GridDataRef->Find(GridPosition)->BuildingOnTile, GridDataRef->Find(GridPosition)->BaseOnTile , GridDataRef->Find(GridPosition)->TowerOnTile));
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

void UGridInfo::addBuildingOnGrid(FIntPoint GridPosition, ABuilding* Building)
{
	BuildingGrid.Add(Building);
	SetBuildingOnGrid(GridPosition, Building);
	
}

void UGridInfo::SetBuildingOnGrid(FIntPoint GridPosition, ABuilding* Building)
{
	if(Grid == nullptr)
	{
		return;
	}

	if(GridPosition != Building->GetGridPosition())
	{
		Building->SetGridPosition(GridPosition);
		if(Building->GetGridPosition() != FIntPoint(-999,-999))
		{
			FDC_TileData* NewIndex = Grid->GetGridData()->Find(Building->GetGridPosition());
			if(NewIndex != nullptr)
			{
				Grid->GetGridData()->Add(NewIndex->TilePosition, FDC_TileData(NewIndex->TilePosition, NewIndex->TileType, NewIndex->TileTransform, NewIndex->TileState, NewIndex->UnitOnTile, Building, NewIndex->BaseOnTile, NewIndex->TowerOnTile));
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
				Grid->GetGridData()->Add(PerviousIndex->TilePosition, FDC_TileData(PerviousIndex->TilePosition, PerviousIndex->TileType, PerviousIndex->TileTransform, PerviousIndex->TileState, PerviousIndex->UnitOnTile, PerviousIndex->BuildingOnTile, nullptr, PerviousIndex->TowerOnTile));
			}
		}
		Base->SetGridPosition(GridPosition);
		if(Base->GetGridPosition() != FIntPoint(-999,-999))
		{
			FDC_TileData* NewIndex = Grid->GetGridData()->Find(Base->GetGridPosition());
			if(NewIndex != nullptr)
			{
				Grid->GetGridData()->Add(NewIndex->TilePosition, FDC_TileData(NewIndex->TilePosition, NewIndex->TileType, NewIndex->TileTransform, NewIndex->TileState, NewIndex->UnitOnTile, NewIndex->BuildingOnTile, Base, NewIndex->TowerOnTile));
			}
		}
	}
}

void UGridInfo::addTowerOnGrid(FIntPoint GridPosition, ATower* Tower)
{
	TowerGrid.Add(Tower);
	SetTowerOnGrid(GridPosition, Tower);
}

void UGridInfo::SetTowerOnGrid(FIntPoint GridPosition, ATower* Tower)
{
	if(Grid == nullptr)
	{
		return;
	}
	if(GridPosition != Tower->GetGridPosition())
	{
		FDC_TileData* PerviousIndex = Grid->GetGridData()->Find(Tower->GetGridPosition());
		Tower->SetGridPosition(GridPosition);
		if(Tower->GetGridPosition() != FIntPoint(-999,-999))
		{
			FDC_TileData* NewIndex = Grid->GetGridData()->Find(Tower->GetGridPosition());
			
			if(NewIndex)
			{
				Grid->GetGridData()->Add(NewIndex->TilePosition, FDC_TileData(NewIndex->TilePosition, NewIndex->TileType, NewIndex->TileTransform, NewIndex->TileState, NewIndex->UnitOnTile, NewIndex->BuildingOnTile, NewIndex->BaseOnTile, Tower));
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

