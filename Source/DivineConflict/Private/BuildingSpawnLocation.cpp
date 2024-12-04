// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingSpawnLocation.h"

#include "Building.h"
#include "Grid.h"
#include "GridPath.h"
#include "GridVisual.h"

	// ----------------------------
	// Initialisation
UBuildingSpawnLocation::UBuildingSpawnLocation()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	BuildingRef = Cast<ABuilding>(GetOwner()); 
	
	if (BuildingRef != nullptr)
	{
		BuildingRef->BuildingSpawnLocationRef = this;
	}
	// ...
}

	// ----------------------------
	// Overrides
void UBuildingSpawnLocation::BeginPlay()
{
	Super::BeginPlay();
	
	// ...
}

void UBuildingSpawnLocation::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

	// ----------------------------
	// Grid Colors
void UBuildingSpawnLocation::SpawnGridColors(TArray<FIntPoint> AllSpawnLocationIndex){
	BuildingRef = Cast<ABuilding>(GetOwner()); 
	if (BuildingRef != nullptr)
	{
		if (AllSpawnLocationIndex.Num() != 0)
		{
			for (FIntPoint Index : AllSpawnLocationIndex)
			{
				GridRef->RemoveInstance(Index);
				GridInstance = GridRef->AddInstance(Index, GridRef->GetGridData()->Find(Index)->TileTransform);
				if (GridRef->GridPath->IsValidHeight(GridRef->GetGridData()->Find(Index), GridRef->GetGridData()->Find(BuildingRef->SpawnLocRef[0])) && GridRef->GetGridData()->Find(Index)->UnitOnTile == nullptr)
				{
					GridRef->GridVisual->AddStateToTile(Index, EDC_TileState::Spawnable);
				}
				else
				{
					GridRef->GridVisual->AddStateToTile(Index, EDC_TileState::NotSpawnable);
				}
			}
		}
	}
}

void UBuildingSpawnLocation::DespawnGridColors(TArray<FIntPoint> AllSpawnLocationIndex)
{
	BuildingRef = Cast<ABuilding>(GetOwner()); 
	if (BuildingRef != nullptr)
	{
		if (AllSpawnLocationIndex.Num() != 0)
		{
			for (FIntPoint Index : AllSpawnLocationIndex)
			{
				GridRef->RemoveInstance(Index);
				GridInstance = GridRef->AddInstance(Index, GridRef->GetGridData()->Find(Index)->TileTransform);
				if (GridRef->GridPath->IsValidHeight(GridRef->GetGridData()->Find(Index), GridRef->GetGridData()->Find(BuildingRef->SpawnLocRef[0])) && GridRef->GetGridData()->Find(Index)->UnitOnTile == nullptr)
				{
					GridRef->GridVisual->RemoveStateFromTile(Index, EDC_TileState::Spawnable);
				}
				else
				{
					GridRef->GridVisual->RemoveStateFromTile(Index, EDC_TileState::NotSpawnable);
				}
			}
		}
		/*
		for (FIntPoint BuildingIndex : BuildingRef->SpawnLocRef)
		{
			GridRef->GridVisual->RemoveStateFromTile(BuildingIndex, EDC_TileState::Selected);
		}
		for (FIntPoint Index : BuildingRef->SpawnLocRef)
		{
			GridRef->GridVisual->RemoveStateFromTile(Index, EDC_TileState::Spawnable);
		}*/
	}
}