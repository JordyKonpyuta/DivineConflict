// Fill out your copyright notice in the Description page of Project Settings.


#include "GridPath.h"

#include "Grid.h"


// Sets default values for this component's properties
UGridPath::UGridPath()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
}


// Called when the game starts
void UGridPath::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


TArray<FIntVector2> UGridPath::FindTileNeighbors(FIntVector2 Index)
{
	TArray<FIntVector2> Neighbors;
	Neighbors.Add(FIntVector2(Index.X + 1, Index.Y));
	Neighbors.Add(FIntVector2(Index.X - 1, Index.Y));
	Neighbors.Add(FIntVector2(Index.X, Index.Y + 1));
	Neighbors.Add(FIntVector2(Index.X, Index.Y - 1));
	return Neighbors;
}

bool UGridPath::IsInputDataValid(FIntVector2 Start, FIntVector2 End)
{
	if(Start == End)
	{
		return false;
	}
	if(Start.X < 0 || Start.Y < 0 || End.X < 0 || End.Y < 0)
	{
		return false;
	}
	if(Start.X >= Grid->GridSize.X || Start.Y >= Grid->GridSize.Y || End.X >= Grid->GridSize.X || End.Y >= Grid->GridSize.Y)
	{
		return false;
	}
	if(!Grid->IsTileWalkable(Start))
	{
		return false;
	}
	if(!Grid->IsTileWalkable(End))
	{
		return false;
	}
	//if(bIsReachable)
	
	//else return true;
	
	return true;
}

void UGridPath::DiscoverTile(FPathData TilePath)
{
	PathData.Add(TilePath.Index, TilePath);
	InserTileDiscoverList(TilePath);
}

int UGridPath::MinimulCostBetweenTwoTile(FIntVector2 Index1, FIntVector2 Index2)
{
	return FMath::Abs(Index1.X - Index2.X) + FMath::Abs(Index1.Y - Index2.Y);
}

bool UGridPath::AnalyseNextDiscoverTile()
{
	CurrentDiscoverTile = PullCheapestTileOutOfDiscoverList();

	CurrentsNeighbors = GetValidTileNeighbors(CurrentDiscoverTile.Index);

	while (CurrentsNeighbors.Num() > 0)
	{
		if(DiscoverNextNeighbors())
		{
			return true;
		}
		
	}
	
	return false;
}

TArray<FIntVector2> UGridPath::GeneratePath()
{
	return TArray<FIntVector2>();
}

FPathData UGridPath::PullCheapestTileOutOfDiscoverList()
{
	FIntVector2 Index = DiscoverTileIndex[0];
	DiscoverTileIndex.RemoveAt(0);
	DiscoverTileSortingCost.RemoveAt(0);

	AnalyseTileIndex.Add(Index);
	
	return PathData[Index];
	
}

bool UGridPath::DiscoverNextNeighbors()
{
	CurrentNeighbors = CurrentsNeighbors[0];
	CurrentsNeighbors.RemoveAt(0);

	if(!AnalyseTileIndex.Contains(CurrentNeighbors.Index))
	{
		int CostFormStart = CurrentDiscoverTile.CostFormStart + CurrentNeighbors.CostFormStart;
		if(CostFormStart >= MaxLenght) return false;
		if (DiscoverTileIndex.Find(CurrentNeighbors.Index) != -1)
		{
			if(PathData[CurrentNeighbors.Index].CostFormStart < CostFormStart)
			{
				return false;
			}
			
		}
		DiscoverTile(FPathData(CurrentNeighbors.Index, CurrentNeighbors.CostFormStart, CostFormStart,
				MinimulCostBetweenTwoTile(CurrentNeighbors.Index, CurrentDiscoverTile.Index)));
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

TArray<FPathData> UGridPath::GetValidTileNeighbors(FIntVector2 Index)
{
	//initialize the neighbors
	TArray<FPathData> Ret;
	
	//get the Grid data for the index
	FDC_TileData TileData = Grid->GetGridData()[Index];
	
	//get the neighbors of the index
	TArray<FIntVector2> Neighbors = FindTileNeighbors(Index);

	//for each neighbor
	for(FIntVector2 Neighbor : Neighbors)
	{
		//get the Grid data for the neighbor
		FDC_TileData NeighborTileData = Grid->GetGridData()[Neighbor];
		
		//check if the neighbor is walkable
		if(!Grid->IsTileTypeWalkable(NeighborTileData.TileType))
		{
			continue;
		}
		if (IsValidHeigh(NeighborTileData, TileData))
		{
			Ret.Add(FPathData(NeighborTileData.TilePosition,1,99999,99999));
		}
	}

	
	//return the neighbors
	
	return Ret;
	

	
}

void UGridPath::InserTileDiscoverList(FPathData TilePath)
{
	int SortingCost = TilePath.MinCostToTarget + TilePath.CostFormStart;

	if(DiscoverTileSortingCost.Num()==0)
	{
		DiscoverTileSortingCost.Add(SortingCost);
		DiscoverTileIndex.Add(TilePath.Index);
	}
	else
	{
		if (SortingCost>= DiscoverTileSortingCost[DiscoverTileSortingCost.Num()-1])
		{
			DiscoverTileSortingCost.Add(SortingCost);
			DiscoverTileIndex.Add(TilePath.Index);
		}
		else
		{
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

void UGridPath::ClearGeneratedPath()
{
	DiscoverTileIndex.Empty();

	DiscoverTileSortingCost.Empty();

	AnalyseTileIndex.Empty();

	PathData.Empty();

	Path.Empty();
}

bool UGridPath::IsValidHeigh(FDC_TileData IndextestData, FDC_TileData CurrentIndexData)
{
	
	return FMath::Abs(IndextestData.TileTransform.GetLocation().Z - CurrentIndexData.TileTransform.GetLocation().Z) <= (100/1.75);

}

// Called every frame
void UGridPath::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

TArray<FIntVector2> UGridPath::FindPath(FIntVector2 Start, FIntVector2 End, bool IsReachable, int PathLenght,
	bool IsEscalation)
{
	//initialize the path
	StartPoint = Start;
	EndPoint = End;
	bIsReachable = IsReachable;
	MaxLenght = PathLenght;
	bIsEscalation = IsEscalation;

	ClearGeneratedPath();

	//check if the input data is valid
	if (!IsInputDataValid(Start, End))
	{
		return TArray<FIntVector2>();
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
	return  TArray<FIntVector2>();
	
	
	
}

