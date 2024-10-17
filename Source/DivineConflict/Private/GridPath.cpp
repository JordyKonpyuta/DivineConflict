// Fill out your copyright notice in the Description page of Project Settings.


#include "GridPath.h"
#include "Building.h"
#include "Grid.h"
#include "Unit.h"

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

	// ----------------------------
	// Tile Discovery (Process)

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

	// ----------------------------
	// Neighbours

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

TArray<FIntPoint> UGridPath::NewFindPath(FIntPoint Start, ACustomPlayerController* CPC)
{

	FIntPoint StartPos = CPC->UnitRef->GetIndexPosition();
	TArray<FIntPoint> AllMoveCases = {StartPos};
	TArray<FIntPoint> NewCases = {StartPos};
	if (CPC->UnitRef->GetIsGarrison())
	{
		AllMoveCases = CPC->UnitRef->GetBuildingRef()->SpawnLocRef;
		NewCases = CPC->UnitRef->GetBuildingRef()->SpawnLocRef;
	}
	TArray<FIntPoint> NewNewCases;
	int AllMovement = CPC->UnitRef->GetPM();

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
	AllMoveCases += NewCases;
	return AllMoveCases;
}

	// ----------------------------
	// SETTER

void UGridPath::SetGrid(AGrid* GridRef)
{
	Grid = GridRef;
}