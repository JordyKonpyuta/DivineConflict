// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"

#include "BuildingSpawnLocation.h"
#include "CustomPlayerController.h"
#include "Grid.h"
#include "GridInfo.h"
#include "InputBehavior.h"
#include "MovieSceneTracksComponentTypes.h"
#include "Unit_Child_Warrior.h"
#include "Unit_Child_Mage.h"
#include "Unit_Child_Tank.h"
#include "GeometryCollection/GeometryCollectionSimulationTypes.h"

// Sets default values
ABuilding::ABuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Add and name a static mesh
	StaticMeshBuilding = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building Appearance"));

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComp;

	StaticMeshBuilding->SetCollisionResponseToAllChannels(ECR_Ignore);

	StaticMeshPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane StaticMesh"));
    StaticMeshPlane->SetHiddenInGame(true);
	StaticMeshPlane->SetupAttachment(RootComponent);
	StaticMeshBuilding->SetupAttachment(RootComponent);
	
}

bool ABuilding::Interact_Implementation(ACustomPlayerController* PlayerController)
{
	return IInteractInterface::Interact_Implementation(PlayerController);
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
			Grid->GridInfo->addBuildingOnGrid(Grid->ConvertLocationToIndex(GetActorLocation()), this);
			Grid->GridInfo->addBuildingOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(100, -100, 0)), this);
			Grid->GridInfo->addBuildingOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(100, 0, 0)), this);
			Grid->GridInfo->addBuildingOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(100, 100, 0)), this);
			Grid->GridInfo->addBuildingOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(0, 100, 0)), this);
			Grid->GridInfo->addBuildingOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(0, -100, 0)), this);
			Grid->GridInfo->addBuildingOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-100, -100, 0)), this);
			Grid->GridInfo->addBuildingOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-100, 0, 0)), this);
			Grid->GridInfo->addBuildingOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-100, 100, 0)), this);
		}
		else
		{
			// Grid : Building
			SpawnLocRef.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(50, 50, 0)));
			SpawnLocRef.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(50, -50, 0)));
			SpawnLocRef.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-50, 50, 0)));
			SpawnLocRef.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-50, -50, 0)));
			Grid->GridInfo->addBuildingOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(50, 50, 0)), this);
			Grid->GridInfo->addBuildingOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(50, -50, 0)), this);
			Grid->GridInfo->addBuildingOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-50, 50, 0)), this);
			Grid->GridInfo->addBuildingOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-50, -50, 0)), this);

			// Grid : Spawners
			AllSpawnLoc.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(150, 50, 0)));
			AllSpawnLoc.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(150, -50, 0)));
			AllSpawnLoc.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(50, 150, 0)));
			AllSpawnLoc.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(50, -150, 0)));
			AllSpawnLoc.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-50, 150, 0)));
			AllSpawnLoc.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-50, -150, 0)));
			AllSpawnLoc.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-150, 50, 0)));
			AllSpawnLoc.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-150, -50, 0)));
			
			BuildingSpawnLocationRef->GridRef = Grid;
			BuildingSpawnLocationRef->SpawnGridColors(AllSpawnLoc);
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

// Check if Player is currently passive; will be used to spawn the HUD
bool ABuilding::IsPlayerPassive(ACustomPlayerController* PlayerController)
{
	if(PlayerController->GetIsInActiveTurn())
	{
		return true;
	}
	else
	{
		return false;
	}
}

// Make a unit spawn in a specific point
void ABuilding::SpawnUnitFromBuilding(FIntPoint SpawnLocation)
{
	FRotator BaseRotation(0.0f, 0.0f, 0.0f);
	switch (UnitProduced)
	{
	case EUnitType::U_Warrior:
		GetWorld()->SpawnActor<AUnit_Child_Warrior>(Grid->ConvertIndexToLocation(SpawnLocation), FRotator(0, 0, 0));
		break;
	case EUnitType::U_Mage:
		GetWorld()->SpawnActor<AUnit_Child_Mage>(Grid->ConvertIndexToLocation(SpawnLocation), FRotator(0, 0, 0));
		break;
	case EUnitType::U_Tank:
		GetWorld()->SpawnActor<AUnit_Child_Tank>(Grid->ConvertIndexToLocation(SpawnLocation), FRotator(0, 0, 0));
		break;
	default:
		GetWorld()->SpawnActor<AUnit_Child_Warrior>(Grid->ConvertIndexToLocation(SpawnLocation), FRotator(0, 0, 0));
	}
	
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

