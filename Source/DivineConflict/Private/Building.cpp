// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"

#include "Grid.h"
#include "GridInfo.h"

// Sets default values
ABuilding::ABuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Add and name a static mesh
	StaticMeshBuilding = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building Appearance"));
	RootComponent = StaticMeshBuilding;
}

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();

	switch (BuildingList) {
	case EBuildingList::B_Wood:
		UnitProduced = EUnitType::U_Mage;
		break;
	case EBuildingList::B_Stone:
		UnitProduced = EUnitType::U_Tank;
		break;
	case EBuildingList::B_Gold:
		UnitProduced = EUnitType::U_Warrior;
		break;
	case EBuildingList::B_AP:
		break;
	default:
		break;
	}

	if (Grid)
	{
		// Create Building Data
		
		if(BuildingList == EBuildingList::B_AP)
		{
			// Grid : Building
			Grid->GridInfo->addSpawnUnitOnGrid(Grid->ConvertLocationToIndex(GetActorLocation()), this);
			Grid->GridInfo->addSpawnUnitOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(100, -100, 0)), this);
			Grid->GridInfo->addSpawnUnitOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(100, 0, 0)), this);
			Grid->GridInfo->addSpawnUnitOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(100, 100, 0)), this);
			Grid->GridInfo->addSpawnUnitOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(0, 100, 0)), this);
			Grid->GridInfo->addSpawnUnitOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(0, -100, 0)), this);
			Grid->GridInfo->addSpawnUnitOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-100, -100, 0)), this);
			Grid->GridInfo->addSpawnUnitOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-100, 0, 0)), this);
			Grid->GridInfo->addSpawnUnitOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-100, 100, 0)), this);
		}
		else
		{
			// Grid : Building
			Grid->GridInfo->addSpawnUnitOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(25, 25, 0)), this);
			Grid->GridInfo->addSpawnUnitOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(25, -25, 0)), this);
			Grid->GridInfo->addSpawnUnitOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-25, 25, 0)), this);
			Grid->GridInfo->addSpawnUnitOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-25, -25, 0)), this);

			// Grid : Spawners
			Grid->GridInfo->addSpawnUnitOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(75, 25, 0)), this);
			Grid->GridInfo->addSpawnUnitOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(75, -25, 0)), this);
			Grid->GridInfo->addSpawnUnitOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(25, 75, 0)), this);
			Grid->GridInfo->addSpawnUnitOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(25, -75, 0)), this);
			Grid->GridInfo->addSpawnUnitOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-25, 75, 0)), this);
			Grid->GridInfo->addSpawnUnitOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-25, -75, 0)), this);
			Grid->GridInfo->addSpawnUnitOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-75, 25, 0)), this);
			Grid->GridInfo->addSpawnUnitOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-75, -25, 0)), this);
		}

		// Access Grid Data :
		// Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(25, 25, 0))).
	}
}

void ABuilding::SpawnUnit()
{
	
}

void ABuilding::SwitchOwner(ACustomPlayerState* NewOwner)
{
	switch (BuildingList)
	{
	case EBuildingList::B_Wood:
		OwnerPlayerState->WoodBuildingOwned -= 1;
		NewOwner->WoodBuildingOwned += 1;
		break;
	case EBuildingList::B_Stone:
		OwnerPlayerState->StoneBuildingOwned -= 1;
		NewOwner->StoneBuildingOwned += 1;
		break;
	case EBuildingList::B_Gold:
		OwnerPlayerState->GoldBuildingOwned -= 1;
		NewOwner->GoldBuildingOwned += 1;
		break;
	case EBuildingList::B_AP:
		OwnerPlayerState->GotCentralBuilding = false;
		NewOwner->GotCentralBuilding = true;
		break;
	default:
		break;
	}
	
	OwnerPlayerState = NewOwner;
}

// Called every frame
void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FIntPoint ABuilding::GetGridPosition()
{
	return GridPosition;
}

void ABuilding::SetGridPosition(FIntPoint GridP)
{
	GridPosition = GridP;
}

