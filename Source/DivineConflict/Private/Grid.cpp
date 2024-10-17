// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid.h"
#include "Base.h"
#include "Building.h"
#include "ClimbActor.h"
#include "GridInfo.h"
#include "GridPath.h"
#include "GridVisual.h"
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
					TArray<EDC_TileState>(), nullptr, nullptr, nullptr, nullptr);
				if(Cast<ABase>(HitResult.GetActor()))
				{
					TileData.TileType = EDC_TileType::Gate;
					UE_LOG( LogTemp, Warning, TEXT("Obstacle"));
				} else if(Cast<AClimbActor>(HitResult.GetActor()))
				{
					TileData.TileType = EDC_TileType::Climbable;
					UE_LOG( LogTemp, Warning, TEXT("Climbable"));
				} else if (Cast<ABuilding>(HitResult.GetActor()))
				{
					TileData.TileType = EDC_TileType::Building;
					UE_LOG( LogTemp, Warning, TEXT("Building"));
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
			UE_LOG(LogTemp, Warning, TEXT("Hit : %s"), *HitOut.GetActor()->GetName());
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

bool AGrid::IsTileWalkable(FIntPoint Index)
{
	return IsTileTypeWalkable(GetGridData()->Find(Index)->TileType);
	
}

bool AGrid::IsTileTypeWalkable(EDC_TileType Type)
{
	TArray<EDC_TileType> WalkableTypes = { EDC_TileType::None, EDC_TileType::Obstacle, EDC_TileType::Gate };

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
	UE_LOG( LogTemp, Warning, TEXT("Index : %d %d"), Index.X, Index.Y);
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
	// Tests

void AGrid::TestPathfinding()
{
	TArray<FIntPoint> Path = GridPath->FindPath(FIntPoint	(0, 0), FIntPoint(9, 9), false,2 ,false);

	for(FIntPoint Index : Path)
	{
		UE_LOG(LogTemp, Warning, TEXT("Path : %d %d"), Index.X, Index.Y);
		GridVisual->addStateToTile(Index,EDC_TileState::Reachable);
	}
}

void AGrid::TestReachedPath()
{
	TArray<FIntPoint> Reach = GridPath->FindPath(FIntPoint(14,3),FIntPoint(-999,-999), true, 2, false);
	for(FIntPoint Index : Reach)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reach not clim : %d %d"), Index.X, Index.Y);
	}
}

void AGrid::TestReachwithCliming()
{
	TArray<FIntPoint> Reach = GridPath->FindPath(FIntPoint(14,3),FIntPoint(-999,-999), true, 2, true);
	for(FIntPoint Index : Reach)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reach with clim : %d %d"), Index.X, Index.Y);
	}
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