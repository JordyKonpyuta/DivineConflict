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


TArray<FIntPoint> UGridPath::FindTileNeighbors(FIntPoint Index)
{
	TArray<FIntPoint> Neighbors;
	Neighbors.Add(FIntPoint(Index.X + 1, Index.Y));
	Neighbors.Add(FIntPoint(Index.X - 1, Index.Y));
	Neighbors.Add(FIntPoint(Index.X, Index.Y + 1));
	Neighbors.Add(FIntPoint(Index.X, Index.Y - 1));
	
	
	return Neighbors;
}

bool UGridPath::IsInputDataValid(FIntPoint Start, FIntPoint End)
{
	if(Start == End)
	{
		return false;
	}
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

int UGridPath::MinimulCostBetweenTwoTile(FIntPoint Index1, FIntPoint Index2)
{
	return FMath::Abs(Index1.X - Index2.X) + FMath::Abs(Index1.Y - Index2.Y);
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

TArray<FIntPoint> UGridPath::GeneratePath()
{
	return TArray<FIntPoint>();
}

FPathData UGridPath::PullCheapestTileOutOfDiscoverList()
{
	FIntPoint Index = DiscoverTileIndex[0];
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
		
		int CostFormStart = CurrentDiscoverTile.CostFormStart + CurrentNeighbors.costEntrerToTile;
		if(CostFormStart >= MaxLenght) return false;
		
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
		if(!Grid->IsTileTypeWalkable(NeighborTileData->TileType))
		{
			continue;
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

void UGridPath::SetGrid(AGrid* GridRef)
{
	Grid = GridRef;
}

bool UGridPath::IsValidHeigh(FDC_TileData* IndextestData, FDC_TileData* CurrentIndexData)
{
	return FMath::Abs(IndextestData->TileTransform.GetLocation().Z - CurrentIndexData->TileTransform.GetLocation().Z) <= (100/1.75);
}

// Called every frame
void UGridPath::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

TArray<FIntPoint> UGridPath::FindPath(FIntPoint Start, FIntPoint End, bool IsReachable, int PathLenght,	bool IsEscalation)
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
		return TArray<FIntPoint>();
	}

	//discover the start tile
	DiscoverTile(FPathData(Start,1 ,MinimulCostBetweenTwoTile(StartPoint,EndPoint),0));

	//while we have not reached the end tile
	while (DiscoverTileIndex.Num() > 0)
	{
		//UE_LOG( LogTemp, Warning, TEXT("DiscoverTileIndex : %d %d"), DiscoverTileIndex[0].X, DiscoverTileIndex[0].Y);
		if(AnalyseNextDiscoverTile())
		{
			return GeneratePath();
		}
			

	}
	if (bIsReachable)
	{
		return AnalyseTileIndex;
	}
	else
	{
		return TArray<FIntPoint>({FIntPoint(-1,-1)});
	}
	
	
	
	
}

