// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingSpawnLocation.h"

#include "Building.h"
#include "Grid.h"
#include "GridPath.h"

// Sets default values for this component's properties
UBuildingSpawnLocation::UBuildingSpawnLocation()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	BuildingRef = Cast<ABuilding>(GetOwner()); 
	
	if (BuildingRef != nullptr)
	{
		BuildingRef->BuildingSpawnLocationRef = this;
		GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Turquoise, TEXT("BuildingRef is Set (1)!"));
	}
	// ...
}


// Called when the game starts
void UBuildingSpawnLocation::BeginPlay()
{
	Super::BeginPlay();
	
	// ...
}

// Called every frame
void UBuildingSpawnLocation::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UBuildingSpawnLocation::SpawnGridColors(TArray<FIntPoint> AllSpawnLocationIndex){
	bool bStopperToDeath = false;
	BuildingRef = Cast<ABuilding>(GetOwner()); 
	if (BuildingRef != nullptr)
	{
		if (AllSpawnLocationIndex.Num() != 0)
		{
			for (FIntPoint Index : AllSpawnLocationIndex)
			{
				GridRef->RemoveInstance(Index);
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Turquoise, TEXT("Index :" + Index.ToString()));
				GridInstance = GridRef->AddInstance(Index, GridRef->GetGridData()->Find(Index)->TileTransform);
				if (GridRef->GridPath->IsValidHeigh(GridRef->GetGridData()->Find(Index), GridRef->GetGridData()->Find(BuildingRef->SpawnLocRef)))
				{
					GridRef->UpdateColor(GridInstance, FLinearColor (0, 255, 0), 0);
					GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Turquoise, TEXT("Colored to Green"));
				}
				else
				{
					GridRef->UpdateColor(GridInstance, FLinearColor (255, 0, 0), 0);
					GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Turquoise, TEXT("Colored to Red"));
				}
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Turquoise, TEXT("AllSpawnLocationIndex is Empty"));
		}
	}
	else
	{
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Turquoise, TEXT("BuildingRef is Quantic! (4)"));
		
	}
}