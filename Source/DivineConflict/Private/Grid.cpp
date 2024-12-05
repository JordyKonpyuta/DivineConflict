// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid.h"
#include "Base.h"
#include "Building.h"
#include "ClimbActor.h"
#include "GridInfo.h"
#include "GridPath.h"
#include "GridVisual.h"
#include "Obstacle.h"
#include "Upwall.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"


	// ----------------------------
	// Constructor

AGrid::AGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    GridMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Grid Mesh"));
	RootComponent = GridMesh;

	GridMesh->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/AssetImport/Square/SM_Grid_SquareFlat.SM_Grid_SquareFlat'")).Object);
	if (GridMesh->GetStaticMesh() == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Static Mesh not found"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Static Mesh found"));
	}

	GridMesh->NumCustomDataFloats = 4;

	GridPath = CreateDefaultSubobject<UGridPath>(TEXT("Grid Path"));
	if(GridPath != nullptr)
	{
		GridPath->Grid = this;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid Path not found"));
	}
	GridInfo = CreateDefaultSubobject<UGridInfo>(TEXT("Grid Info"));
	if(GridInfo != nullptr)
	{
		GridInfo->SetGrid(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid Info not found"));
	}
	GridVisual = CreateDefaultSubobject<UGridVisual>("Grid Visual");
	if(GridVisual != nullptr)
	{
		GridVisual->SetGrid(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid Utilities not found"));
	}

	SpawnGrid();
	
}

	// ----------------------------
	// Overrides

void AGrid::BeginPlay()
{
	Super::BeginPlay();


}

void AGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	
}

	// ----------------------------
	// Spawn Grid

void AGrid::SpawnGrid()
{

	int X = 0;
	int Y = 0;
	GridMesh->ClearInstances();
	InstanceArray.Empty();
	while (X < GridSize.X)
	{
		while (Y < GridSize.Y)
		{
			FHitResult HitResult = TraceHitGround(FVector(X * 100, Y * 100, 0));
			if (HitResult.ImpactPoint != FVector(0, 0, 0))
			{
				FIntPoint TileIndex = FIntPoint(X, Y);
				FDC_TileData TileData = FDC_TileData(TileIndex, EDC_TileType::Normal, FTransform3d(FVector(X * 100, Y * 100, HitResult.ImpactPoint.Z)),
					TArray<EDC_TileState>(), nullptr, nullptr, nullptr, nullptr, nullptr);
				if(Cast<ABase>(HitResult.GetActor()))
				{
					TileData.TileType = EDC_TileType::Gate;
				} else if(Cast<AUpwall>(HitResult.GetActor()))
				{
					TileData.TileType = EDC_TileType::Climbable;
				} else if (Cast<ABuilding>(HitResult.GetActor()))
				{
					TileData.TileType = EDC_TileType::Building;
				} else if (Cast<AObstacle>(HitResult.GetActor()))
				{
					TileData.TileType = EDC_TileType::Obstacle;
				}

				
				GridData.Add(TileIndex, TileData);
				//SetGridDataReplicated(FGRidData(GridData));
	            AddInstance(FIntPoint(X, Y), FTransform3d(FVector(X * 100, Y * 100, HitResult.ImpactPoint.Z+1)));

		
			}
			Y++;
		}
		Y = 0;
		X++;
	}

	GridInfo->SetGrid(this);
	GridPath->SetGrid(this);
	GridVisual->SetGrid(this);
	
}

FHitResult AGrid::TraceHitGround(FVector Location)
{
	FHitResult HitOut;
	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Location + FVector(0, 0, 1000), Location + FVector(0, 0, -1000), 10,
		ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, HitOut, true);
	if (HitOut.bBlockingHit)
	{
		if(HitOut.GetActor() != nullptr)
		HitOut.bBlockingHit ? DrawDebugPoint(GetWorld(), HitOut.ImpactPoint, 10, FColor::Green, false,
		1, 0) : DrawDebugPoint(GetWorld(), Location + FVector(0, 0, -1000), 10, FColor::Red, false, 1, 0);
		return HitOut;
	}
		return FHitResult();
}

FVector AGrid::SnapVectorToVector(FVector InVector, const FVector InSnapTo)
{
	return FVector(UKismetMathLibrary::GridSnap_Float(InVector.X, InSnapTo.X), UKismetMathLibrary::GridSnap_Float(InVector.Y, InSnapTo.Y), UKismetMathLibrary::GridSnap_Float(InVector.Z, InSnapTo.Z));
}

	// ----------------------------
	// Check Grid Elements

bool AGrid::IsTileWalkable(FIntPoint Index,bool Attacking)
{
	if (Attacking)
	{
		return CanAttackTileType(GetGridData()->Find(Index)->TileType);
	}
	return IsTileTypeWalkable(GetGridData()->Find(Index)->TileType);
}


bool AGrid::IsTileTypeWalkable(EDC_TileType Type)
{
	TArray<EDC_TileType> WalkableTypes = { EDC_TileType::None, EDC_TileType::Obstacle, EDC_TileType::Gate, EDC_TileType::Tower };

	return !WalkableTypes.Contains(Type);
}

bool AGrid::CanAttackTileType(EDC_TileType Type)
{
	TArray<EDC_TileType> WalkableTypes = { EDC_TileType::None, EDC_TileType::Obstacle , EDC_TileType::Tower};

	return !WalkableTypes.Contains(Type);
}

// ----------------------------
	// Converts

FIntPoint AGrid::ConvertLocationToIndex(FVector3d Location)
{

	FVector3d TempLoc = (SnapVectorToVector(Location, TileSize))/TileSize;
	return FIntPoint(TempLoc.X, TempLoc.Y);
}

FVector3d AGrid::ConvertIndexToLocation(FIntPoint Index)
{
	return GridData.Find(Index)->TileTransform.GetLocation();
}


	// ----------------------------
	// Instance Editors

int AGrid::AddInstance(FIntPoint Index, FTransform3d Transform)
{
	GridMesh->AddInstance(Transform,true);

	return InstanceArray.Add(Index);
	
}

void AGrid::RemoveInstance(FIntPoint Index)
{

	if (InstanceArray.Contains(Index))
	{
		GridMesh->RemoveInstance(InstanceArray.Find(Index));
		InstanceArray.Remove(Index);
	}
}

	// ----------------------------
	// Color

void AGrid::UpdateColor(int I, FLinearColor InColor, float	Alpha)
{
	GridMesh->SetCustomDataValue(I, 0, InColor.R);
	GridMesh->SetCustomDataValue(I, 1, InColor.G);
	GridMesh->SetCustomDataValue(I, 2, InColor.B);
	GridMesh->SetCustomDataValue(I, 3, Alpha);
}

	// ----------------------------
	// GETTERS

TMap<FIntPoint, FDC_TileData>* AGrid::GetGridData()
{
	return &GridData;
}

	// ----------------------------
	// SETTERS

void AGrid::SetGridData(TMap<FIntPoint, FDC_TileData> Data)
{
	GridData = Data;
}