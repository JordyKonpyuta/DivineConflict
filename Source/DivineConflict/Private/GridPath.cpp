// Fill out your copyright notice in the Description page of Project Settings.


#include "GridPath.h"
#include "Building.h"
#include "Grid.h"
#include "Unit.h"
#include "Unit_Child_Warrior.h"

// ----------------------------
	// Constructor
UGridPath::UGridPath()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
}

	// ----------------------------
	// Overrides

void UGridPath::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UGridPath::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

	// ----------------------------
	// Tile Discovery (Set-Up)

void UGridPath::DiscoverTile(FPathData TilePath)
{
	PathData.Add(TilePath.Index, TilePath);
	InserTileDiscoverList(TilePath);
}

int UGridPath::MinimulCostBetweenTwoTile(FIntPoint Index1, FIntPoint Index2)
{
	return FMath::Abs(Index1.X - Index2.X) + FMath::Abs(Index1.Y - Index2.Y);
}

void UGridPath::InserTileDiscoverList(FPathData TilePath)
{
	//stock the sorting cost of the tile
	int SortingCost = TilePath.MinCostToTarget + TilePath.CostFormStart;
	//if the list is empty
	if(DiscoverTileSortingCost.Num()==0)
	{
		DiscoverTileSortingCost.Add(SortingCost);
		DiscoverTileIndex.Add(TilePath.Index);
	}
	else
	{
		//if the sorting cost is higher than the last element of the list
		if (SortingCost>= DiscoverTileSortingCost[DiscoverTileSortingCost.Num()-1])
		{
			DiscoverTileSortingCost.Add(SortingCost);
			DiscoverTileIndex.Add(TilePath.Index);
		}
		else
		{
			//if the sorting cost is lower than the first element of the list
			for(int i = 0; i<DiscoverTileSortingCost.Num();i++)
			{
				if(SortingCost<=DiscoverTileSortingCost[i])
				{
					DiscoverTileSortingCost.Insert(SortingCost, i);
					DiscoverTileIndex.Insert(TilePath.Index, i);
					break;
				}
			}
		}
	}
}

bool UGridPath::IsInputDataValid(FIntPoint Start, FIntPoint End)
{
	//check if the start and end are the same
	if(Start == End)
	{
		return false;
	}
	//check if the start and end are in the grid
	if(Start.X < 0 || Start.Y < 0  )
	{
		return false;
	}
	if(bIsReachable)
		return true;
	if(End.X < 0 || End.Y < 0)
		return false;
	if(Start.X >= Grid->GridSize.X || Start.Y >= Grid->GridSize.Y || End.X >= Grid->GridSize.X || End.Y >= Grid->GridSize.Y)
	{
		return false;
	}
	//check if the start and end are walkable
	if(!Grid->IsTileWalkable(Start, bIsAttackable))
	{
		return false;
	}
	if(!Grid->IsTileWalkable(End, bIsAttackable))
	{
		return false;
	}

	return true;
}

	// ----------------------------
	// Get a New Tile for Analyse

FPathData UGridPath::PullCheapestTileOutOfDiscoverList()
{
	FIntPoint Index = DiscoverTileIndex[0];
	DiscoverTileIndex.RemoveAt(0);
	DiscoverTileSortingCost.RemoveAt(0);

	AnalyseTileIndex.Add(Index);
	
	return PathData[Index];
	
}

bool UGridPath::AnalyseNextDiscoverTile()
{
	CurrentDiscoverTile = PullCheapestTileOutOfDiscoverList();

	CurrentsNeighbors = GetValidTileNeighbors(CurrentDiscoverTile.Index);
	
	while (CurrentsNeighbors.Num() > 0)
	{
		//UE_LOG( LogTemp, Warning, TEXT("Current Neighbors : %d %d"), CurrentsNeighbors[0].Index.X, CurrentsNeighbors[0].Index.Y);
		if(DiscoverNextNeighbors())
		{
			return true;
		}
		
	}
	return false;
}

	// ----------------------------
// Height Check

bool UGridPath::IsValidHeigh(FDC_TileData* IndextestData, FDC_TileData* CurrentIndexData)
{
	return FMath::Abs(IndextestData->TileTransform.GetLocation().Z - CurrentIndexData->TileTransform.GetLocation().Z) <= (100/1.75);
}

bool UGridPath::IsValidHeighWarrior(FDC_TileData* IndextestData, FDC_TileData* CurrentIndexData)
{
	if (IndextestData->TileTransform.GetLocation().Z - CurrentIndexData->TileTransform.GetLocation().Z >= -100/1.75 && IndextestData->TileTransform.GetLocation().Z - CurrentIndexData->TileTransform.GetLocation().Z <= 200/1.75)
		return true;
	
	return false;
}

	// ----------------------------
	// Neighbours

bool UGridPath::DiscoverNextNeighbors()
{
	CurrentNeighbors = CurrentsNeighbors[0];
	CurrentsNeighbors.RemoveAt(0);

	// if the neighbor is not already in the AnalyseTileIndex
	if(!AnalyseTileIndex.Contains(CurrentNeighbors.Index))
	{
		
		int CostFormStart = CurrentDiscoverTile.CostFormStart + CurrentNeighbors.costEntrerToTile;
		if(CostFormStart >= MaxLenght) return false;

		// if the neighbor is not already in the DiscoverTileIndex or if the cost to reach the neighbor is lower than the previous cost
		if (DiscoverTileIndex.Find(CurrentNeighbors.Index) == INDEX_NONE)
		{
			if (PathData.Find(CurrentNeighbors.Index))
			{
				if(PathData.Find(CurrentNeighbors.Index)->costEntrerToTile < CostFormStart)
				{
					return false;
				}
			}
		}

		// if the neighbor is the end tile
		DiscoverTile(FPathData(CurrentNeighbors.Index, CurrentNeighbors.CostFormStart, MinimulCostBetweenTwoTile(CurrentNeighbors.Index, CurrentDiscoverTile.Index), CostFormStart));
		if(CurrentNeighbors.Index == EndPoint)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

TArray<FPathData> UGridPath::GetValidTileNeighbors(FIntPoint Index)
{
	//initialize the neighbors
	TArray<FPathData> Ret;

	//get the Grid data for the index
	FDC_TileData* TileData = Grid->GetGridData()->Find(Index);
	
	//get the neighbors of the index
	TArray<FIntPoint> Neighbors = FindTileNeighbors(Index);

	//for each neighbor
	for(FIntPoint Neighbor : Neighbors)
	{
		//check if the neighbor is in the grid
		if(Neighbor.X < 0 || Neighbor.Y < 0)
		{
			continue;
		}
		//get the Grid data for the neighbor

		FDC_TileData* NeighborTileData = Grid->GetGridData()->Find(Neighbor);
		//check if the neighbor is walkable
		if(bIsAttackable)
		{
			if(!Grid->IsTileTypeAttackable(NeighborTileData->TileType))
			{
				continue;
			}
		}
		else
		{
			if(!Grid->IsTileTypeWalkable(NeighborTileData->TileType))
			{
				continue;
			}
		}

		if(NeighborTileData->TileType == EDC_TileType::Climbable && bIsEscalation)
		{
			UE_LOG(LogTemp, Warning, TEXT("Climbable"));
			Ret.Add(FPathData(NeighborTileData->TilePosition,1,99999,99999));
		}
		//check if the heigh is valid
		if (IsValidHeigh(NeighborTileData, TileData))
		{
			Ret.Add(FPathData(NeighborTileData->TilePosition,1,99999,99999));
		}
	}
	//return the neighbors
	return Ret;
}

	// ----------------------------
	// Find Neighbours
TArray<FIntPoint> UGridPath::FindTileNeighbors(FIntPoint Index)
{
	TArray<FIntPoint> Neighbors;
	if(Index.X < Grid->GridSize.X-1) Neighbors.Add(FIntPoint(Index.X + 1, Index.Y));
	if (Index.X > 0){Neighbors.Add(FIntPoint(Index.X - 1, Index.Y));}
	if(Index.Y < Grid->GridSize.Y-1) Neighbors.Add(FIntPoint(Index.X, Index.Y + 1));
	if (Index.Y > 0){Neighbors.Add(FIntPoint(Index.X, Index.Y - 1));}
	
	
	return Neighbors;
}

	// ----------------------------
	// Path

TArray<FIntPoint> UGridPath::GeneratePath()
{
	return TArray<FIntPoint>();
}

	// ----------------------------
	// Clear
void UGridPath::ClearGeneratedPath()
{
	DiscoverTileIndex.Empty();

	DiscoverTileSortingCost.Empty();

	AnalyseTileIndex.Empty();

	PathData.Empty();

	Path.Empty();
}

	// ----------------------------
	// Movement

TArray<FIntPoint> UGridPath::FindPath(FIntPoint Start, FIntPoint End, bool IsReachable, int PathLenght,	bool IsEscalation, bool attackable)
{
	//initialize the path
	StartPoint = Start;
	EndPoint = End;
	bIsReachable = IsReachable;
	MaxLenght = PathLenght;
	bIsEscalation = IsEscalation;
	bIsAttackable = attackable;

	ClearGeneratedPath();

	//check if the input data is valid
	if (!IsInputDataValid(Start, End))
	{
		return TArray<FIntPoint>();
	}

	//discover the start tile
	DiscoverTile(FPathData(Start,1 ,MinimulCostBetweenTwoTile(StartPoint,EndPoint),0));

	//while we have not reached the end tile
	while (DiscoverTileIndex.Num() > 0)
	{
		
		if(AnalyseNextDiscoverTile())
		{
			return GeneratePath();
		}
			

	}
	//if we have not reached the end tile, return an empty path
	if (bIsReachable)
	{
		return AnalyseTileIndex;
	}
	else
	{
		return TArray<FIntPoint>({FIntPoint(-1,-1)});
	}
	
	
	
	
}

TArray<FIntPoint> UGridPath::NewFindPath(FIntPoint Start, ACustomPlayerController* CPC)
{
	TArray<FIntPoint> AllMoveCases = {Start};
	TArray<FIntPoint> NewCases = {Start};
	if (CPC->UnitRef->GetIsGarrison())
	{
		AllMoveCases = CPC->UnitRef->GetBuildingRef()->SpawnLocRef;
		NewCases = CPC->UnitRef->GetBuildingRef()->SpawnLocRef;
	}
	TArray<FIntPoint> NewNewCases;
	int AllMovement = CPC->UnitRef->GetPM();
	bool IsWarrior = Cast<AUnit_Child_Warrior>(CPC->UnitRef) != nullptr;
	TMap<FIntPoint, FDC_TileData>* GridData = Grid->GetGridData();

	if(IsWarrior)
	{
		for (int i = 0; i < AllMovement; i++)
		{
			for (FIntPoint CaseToCheck : NewCases)
			{
				for (FIntPoint NeighbourToCheck : Grid->GridPath->FindTileNeighbors(CaseToCheck))
				{
					if (AllMoveCases.Find(NeighbourToCheck)
						&& Grid->GridPath->IsValidHeighWarrior(Grid->GetGridData()->Find(CaseToCheck), Grid->GetGridData()->Find(NeighbourToCheck)) 
						&& Grid->GetGridData()->Find(NeighbourToCheck)->BuildingOnTile 
						)
					{
						AllMoveCases += CPC->Grid->GetGridData()->Find(NeighbourToCheck)->BuildingOnTile->SpawnLocRef;
					}
					else if (AllMoveCases.Find(NeighbourToCheck)
						&& Grid->GridPath->IsValidHeighWarrior(Grid->GetGridData()->Find(CaseToCheck), Grid->GetGridData()->Find(NeighbourToCheck))
						&& Grid->IsTileWalkable(NeighbourToCheck,false)
						)
					{
						NewNewCases.Add(NeighbourToCheck);
					}
					else
					{
					}
				}
			}
			AllMoveCases += NewCases;
			NewCases = NewNewCases;
			NewNewCases.Empty();
		}
	} else
	{
		for (int i = 0; i < AllMovement; i++)
		{
			for (FIntPoint CaseToCheck : NewCases)
			{
				for (FIntPoint NeighbourToCheck : Grid->GridPath->FindTileNeighbors(CaseToCheck))
				{
					if (AllMoveCases.Find(NeighbourToCheck)
						&& Grid->GridPath->IsValidHeigh(Grid->GetGridData()->Find(CaseToCheck), Grid->GetGridData()->Find(NeighbourToCheck)) 
						&& Grid->GetGridData()->Find(NeighbourToCheck)->BuildingOnTile 
						)
					{
						AllMoveCases += CPC->Grid->GetGridData()->Find(NeighbourToCheck)->BuildingOnTile->SpawnLocRef;
					}
					else if (AllMoveCases.Find(NeighbourToCheck)
						&& Grid->GridPath->IsValidHeigh(Grid->GetGridData()->Find(CaseToCheck), Grid->GetGridData()->Find(NeighbourToCheck))
						&& Grid->IsTileWalkable(NeighbourToCheck,false)
						)
					{
						NewNewCases.Add(NeighbourToCheck);
					}
					else
					{
					}
				}
			}
			AllMoveCases += NewCases;
			NewCases = NewNewCases;
			NewNewCases.Empty();
		}
	}
	
	AllMoveCases += NewCases;
	return AllMoveCases;
}

	// ----------------------------
	// SETTER

void UGridPath::SetGrid(AGrid* GridRef)
{
	Grid = GridRef;
}