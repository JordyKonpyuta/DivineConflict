// Fill out your copyright notice in the Description page of Project Settings.


#include "GridVisual.h"
#include "Grid.h"


	// ----------------------------
	// Constructor

UGridVisual::UGridVisual()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

	// ----------------------------
	// Overrides

void UGridVisual::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UGridVisual::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

	// ----------------------------
	// Get Colors

FLinearColor UGridVisual::GetColorFromState(TArray<EDC_TileState> TileState)
{
	if(TileState.Num() == 0 )
		return FLinearColor(0,0,0,0);
	else
	{
		TArray<EDC_TileState> ArrayMake = {EDC_TileState::Selected, EDC_TileState::Hovered, EDC_TileState::Pathfinding, EDC_TileState::Reachable,EDC_TileState::Attacked, EDC_TileState::Spawnable, EDC_TileState::NotSpawnable};
		for(EDC_TileState i : ArrayMake)
		{
			if(TileState.Contains(i))
			{
				switch (i)
				{
					case EDC_TileState::Selected:
						return FLinearColor(0,0,1,1);
					case EDC_TileState::Hovered:
						return FLinearColor(0,1,0,1);
					case EDC_TileState::Attacked:
						return FLinearColor(1,0,1,1);
					case EDC_TileState::Pathfinding:
						return FLinearColor(1,0,0,1);
					case EDC_TileState::Reachable:
						return FLinearColor(1,1,0,1);
					case EDC_TileState::Spawnable:
						return FLinearColor(0,0.35,0,1);
					case EDC_TileState::NotSpawnable:
						return FLinearColor(0.55, 0,0,1);
					case EDC_TileState::None:
						return FLinearColor(0,0,0,0);
				}
			}
		}
	}
	return FLinearColor(0,0,0,0);
}

	// ----------------------------
	// Set Colors

void UGridVisual::UpdateVisuals(FIntPoint Index)
{
	
	Grid->RemoveInstance(Index);
	int32 i =Grid->AddInstance(Index, Grid->GetGridData()->Find(Index)->TileTransform);


	Grid->UpdateColor(i, GetColorFromState(Grid->GetGridData()->Find(Index)->TileState),GetColorFromState(Grid->GetGridData()->Find(Index)->TileState).A);	
	
}

	// ----------------------------
	// State Modifier

void UGridVisual::AddStateToTile(FIntPoint Index, EDC_TileState TileState)
{
	if(Grid)
	{
		if(Grid->GetGridData()->Find(Index))
		{
			FDC_TileData* Data = Grid->GetGridData()->Find(Index);
			Data->TileState.Add(TileState);
			Grid->GetGridData()->Add(Index, *Data);
			UpdateVisuals(Index);
			
		}
	}
}

void UGridVisual::RemoveStateFromTile(FIntPoint Index, EDC_TileState TileState)
{
	if (Grid)
	{
		if(Grid->GetGridData()->Find(Index))
		{
			FDC_TileData* Data = Grid->GetGridData()->Find(Index);
			Data->TileState.Remove(TileState);
			Grid->GetGridData()->Add(Index, *Data);
			UpdateVisuals(Index);
		}
	}
}

void UGridVisual::RemoveAllStateFromTile(FIntPoint Index)
{
	if (Grid)
	{
		if(Grid->GetGridData()->Find(Index))
		{
			FDC_TileData* Data = Grid->GetGridData()->Find(Index);
			
			Data->TileState.Remove(EDC_TileState::Selected);
			Data->TileState.Remove(EDC_TileState::Reachable);
			Data->TileState.Remove(EDC_TileState::Pathfinding);
			Data->TileState.Remove(EDC_TileState::Attacked);
			Data->TileState.Remove(EDC_TileState::Spawned);
			
			Grid->GetGridData()->Add(Index, *Data);
			UpdateVisuals(Index);
		}
	}
}

// ----------------------------
	// SETTER

void UGridVisual::SetGrid(AGrid* GridRef)
{
	Grid = GridRef;
}