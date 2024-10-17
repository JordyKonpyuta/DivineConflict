// Fill out your copyright notice in the Description page of Project Settings.


#include "GridInfo.h"
#include "Base.h"
#include "Building.h"
#include "Grid.h"
#include "F_DC_TileData.h"
#include "Tower.h"
#include "Unit.h"

	// ----------------------------
	// Constructor

UGridInfo::UGridInfo()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}

	// ----------------------------
	// Overrides

void UGridInfo::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UGridInfo::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

	// ----------------------------
	// Unit on Grid

void UGridInfo::AddUnitInGrid(FIntPoint GridPosition, AUnit* Unit)
{
	UnitsCombat.Add(Unit);

	//Server_setUnitIndexOnGrid(GridPosition, Unit);
	Multi_setUnitIndexOnGrid(GridPosition, Unit);
}

void UGridInfo::Multi_setUnitIndexOnGrid_Implementation(const FIntPoint GridPosition, const AUnit* Unit)
{
	if(Grid == nullptr)
	{
		return;
	}

	AUnit* UnitRef = const_cast<AUnit*>(Unit);
	//TMap<FIntPoint,FDC_TileData> GridDataRef = Grid->GetGridData();
	if(GridPosition != UnitRef->GetIndexPosition())
	{
		FDC_TileData* PreviousIndex = Grid->GetGridData()->Find(UnitRef->GetIndexPosition());
		
		if(PreviousIndex != nullptr)
		{
			if(PreviousIndex->UnitOnTile == Unit)
			{
				Grid->GetGridData()->Add(PreviousIndex->TilePosition, FDC_TileData(PreviousIndex->TilePosition, PreviousIndex->TileType, PreviousIndex->TileTransform, PreviousIndex->TileState, nullptr, PreviousIndex->BuildingOnTile , PreviousIndex->BaseOnTile, PreviousIndex->TowerOnTile));
				Grid->GridData.Add(PreviousIndex->TilePosition, FDC_TileData(PreviousIndex->TilePosition, PreviousIndex->TileType, PreviousIndex->TileTransform, PreviousIndex->TileState, nullptr, PreviousIndex->BuildingOnTile , PreviousIndex->BaseOnTile, PreviousIndex->TowerOnTile));
			}
		}
		UnitRef->SetIndexPosition(GridPosition);

		if(UnitRef->GetIndexPosition() != FIntPoint(-999,-999))
		{

			if(Grid->GetGridData()->Find(GridPosition) != nullptr)
			{
				
				Grid->GetGridData()->Add(Grid->GetGridData()->Find(GridPosition)->TilePosition, FDC_TileData(Grid->GetGridData()->Find(GridPosition)->TilePosition, Grid->GetGridData()->Find(GridPosition)->TileType,
					Grid->GetGridData()->Find(GridPosition)->TileTransform, Grid->GetGridData()->Find(GridPosition)->TileState, UnitRef, Grid->GetGridData()->Find(GridPosition)->BuildingOnTile,
					Grid->GetGridData()->Find(GridPosition)->BaseOnTile, Grid->GetGridData()->Find(GridPosition)->TowerOnTile));

				
			}

		}
	}
}

void UGridInfo::Server_setUnitIndexOnGrid_Implementation(const FIntPoint GridPosition, const AUnit* Unit)
{
	Multi_setUnitIndexOnGrid(GridPosition, Unit);
}

void UGridInfo::RemoveUnitInGrid(AUnit* Unit)
{
	UnitsCombat.Remove(Unit);
	Server_setUnitIndexOnGrid(FIntPoint(-999,-999), Unit);
}

	// ----------------------------
	// Building on Grid

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
	TMap<FIntPoint,FDC_TileData>* GridDataRef = Grid->GetGridData();
	if(GridPosition != Building->GetGridPosition())
	{
		Building->SetGridPosition(GridPosition);
		if(Building->GetGridPosition() != FIntPoint(-999,-999))
		{
			FDC_TileData* NewIndex = GridDataRef->Find(Building->GetGridPosition());
			if(NewIndex != nullptr)
			{
				GridDataRef->Add(NewIndex->TilePosition, FDC_TileData(NewIndex->TilePosition, NewIndex->TileType, NewIndex->TileTransform, NewIndex->TileState, NewIndex->UnitOnTile, Building, NewIndex->BaseOnTile, NewIndex->TowerOnTile));
				Grid->GridData.Add(NewIndex->TilePosition, FDC_TileData(NewIndex->TilePosition, NewIndex->TileType, NewIndex->TileTransform, NewIndex->TileState, NewIndex->UnitOnTile, Building, NewIndex->BaseOnTile, NewIndex->TowerOnTile));
			}
			
		}
	}
	
}

	// ----------------------------
	// Base on Grid

void UGridInfo::addBaseOnGrid(FIntPoint GridPosition, ABase* Base)
{
	BasesGrid.Add(Base);
	Multi_SetBaseOnGrid(GridPosition, Base);
}

void UGridInfo::SetBaseOnGrid(FIntPoint GridPosition, ABase* Base)
{
	if(Grid == nullptr)
	{
		return;
	}
	TMap<FIntPoint,FDC_TileData>* GridDataRef = Grid->GetGridData();
	if(GridPosition != Base->GetGridPosition())
	{

		Base->SetGridPosition(GridPosition);
		if(Base->GetGridPosition() != FIntPoint(-999,-999))
		{
			FDC_TileData* NewIndex = Grid->GetGridData()->Find(Base->GetGridPosition());
			if(NewIndex != nullptr)
			{
				GridDataRef->Add(NewIndex->TilePosition, FDC_TileData(NewIndex->TilePosition, NewIndex->TileType, NewIndex->TileTransform, NewIndex->TileState, NewIndex->UnitOnTile, NewIndex->BuildingOnTile, Base, NewIndex->TowerOnTile));
				Grid->GridData.Add(NewIndex->TilePosition, FDC_TileData(NewIndex->TilePosition, NewIndex->TileType, NewIndex->TileTransform, NewIndex->TileState, NewIndex->UnitOnTile, NewIndex->BuildingOnTile, Base, NewIndex->TowerOnTile));
			}
		}
	}
}
 
void UGridInfo::Multi_SetBaseOnGrid_Implementation(const FIntPoint GridPosition, const ABase* BaseRef)
{
	if(Grid == nullptr)
	{
		return;
	}
	TMap<FIntPoint,FDC_TileData>* GridDataRef = Grid->GetGridData();
	ABase* Base = const_cast<ABase*>(BaseRef);
	//if(GridPosition != Base->GetGridPosition())
	{
		FDC_TileData* PreviousIndex = Grid->GetGridData()->Find(Base->GetGridPosition());
		
		if(PreviousIndex != nullptr)
		{
			if(PreviousIndex->BaseOnTile == Base)
			{
				Grid->GetGridData()->Add(PreviousIndex->TilePosition, FDC_TileData(PreviousIndex->TilePosition, PreviousIndex->TileType, PreviousIndex->TileTransform, PreviousIndex->TileState, PreviousIndex->UnitOnTile, PreviousIndex->BuildingOnTile , nullptr, PreviousIndex->TowerOnTile));
				Grid->GridData.Add(PreviousIndex->TilePosition, FDC_TileData(PreviousIndex->TilePosition, PreviousIndex->TileType, PreviousIndex->TileTransform, PreviousIndex->TileState, PreviousIndex->UnitOnTile, PreviousIndex->BuildingOnTile , nullptr, PreviousIndex->TowerOnTile));
			}
		}
		
		Base->SetGridPosition(GridPosition);
		if(Base->GetGridPosition() != FIntPoint(-999,-999))
		{
			
			FDC_TileData* NewIndex = Grid->GetGridData()->Find(GridPosition);
			if(NewIndex != nullptr)
			{
				GridDataRef->Add(NewIndex->TilePosition, FDC_TileData(NewIndex->TilePosition, NewIndex->TileType, NewIndex->TileTransform, NewIndex->TileState, NewIndex->UnitOnTile, NewIndex->BuildingOnTile, Base, NewIndex->TowerOnTile));
				Grid->GridData.Add(NewIndex->TilePosition, FDC_TileData(NewIndex->TilePosition, NewIndex->TileType, NewIndex->TileTransform, NewIndex->TileState, NewIndex->UnitOnTile, NewIndex->BuildingOnTile, Base, NewIndex->TowerOnTile));
			}
		}
	}
}

	// ----------------------------
	// Tower on Grid

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
				Grid->GridData.Add(NewIndex->TilePosition, FDC_TileData(NewIndex->TilePosition, NewIndex->TileType, NewIndex->TileTransform, NewIndex->TileState, NewIndex->UnitOnTile, NewIndex->BuildingOnTile, NewIndex->BaseOnTile, Tower));
			}
		}
	}	
}

	// ----------------------------
	// SETTERS

void UGridInfo::SetGrid(AGrid* GridRef)
{
	Grid = GridRef;
}