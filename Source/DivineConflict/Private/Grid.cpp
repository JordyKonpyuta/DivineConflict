// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid.h"

#include "GridInfo.h"
#include "GridPath.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
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

	
	SpawnGrid();
	
}

bool AGrid::Interact_Implementation(ACustomPlayerController* PlayerController)
{

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Interact grid"));
	return IInteractInterface::Interact_Implementation(PlayerController);
}

// Called when the game starts or when spawned
void AGrid::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector3d AGrid::TraceHitGround(FVector Location)
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
		return HitOut.ImpactPoint;
	}
		return FVector(0, 0, 0);
}



void AGrid::SpawnGrid()
{
	
	int X = 0;
	int Y = 0;
	GridMesh->ClearInstances();
	while (X < GridSize.X)
	{
		while (Y < GridSize.Y)
		{
			FVector3d HitLocation = TraceHitGround(FVector(X * 100, Y * 100, 0));
			if (HitLocation != FVector(0, 0, 0))
			{
				FVector2d TileIndex = FVector2d(X, Y);
				FDC_TileData TileData = FDC_TileData(TileIndex, E_DC_TileTypp::Normal, FTransform3d(FVector(X * 100, Y * 100, HitLocation.Z)),
					std::vector<E_DC_TileState>(), nullptr, nullptr);

				
				GridData.Add(TileIndex, TileData);
				
				GridMesh->AddInstance(FTransform(FVector(X * 100, Y * 100, HitLocation.Z)), true);
			}
			Y++;
		}
		Y = 0;
		X++;
	}
	
}

void AGrid::TestPathfinding()
{
	TArray<FVector2d> Path = GridPath->FindPath(FVector2d(0, 0), FVector2d(9, 9), false,20 ,false);

	for(FVector2d Index : Path)
	{
		UE_LOG(LogTemp, Warning, TEXT("Path : %s"), *Index.ToString());
	}
}

FVector AGrid::SnapVectorToVector(FVector InVector, const FVector InSnapTo)
{
	return FVector(UKismetMathLibrary::GridSnap_Float(InVector.X, InSnapTo.X), UKismetMathLibrary::GridSnap_Float(InVector.Y, InSnapTo.Y), UKismetMathLibrary::GridSnap_Float(InVector.Z, InSnapTo.Z));
}

// Called every frame
void AGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	
}

bool AGrid::IsTileWalkable(FVector2D Index)
{
	return IsTileTypeWalkable(GridData.Find(Index)->TileType);
	
}

bool AGrid::IsTileTypeWalkable(E_DC_TileTypp Type)
{
	TArray<E_DC_TileTypp> WalkableTypes = { E_DC_TileTypp::None, E_DC_TileTypp::Obstacle, E_DC_TileTypp::Gate };

	return !WalkableTypes.Contains(Type);
	
}

TMap<FVector2D, FDC_TileData> AGrid::GetGridData()
{
	return GridData;
}

FVector2d AGrid::ConvertLocationToIndex(FVector Location)
{
	
	FVector testlocation = SnapVectorToVector(Location, TileSize);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Location: %s"), *testlocation.ToString()));
	
	FVector TempLoc = (SnapVectorToVector(Location, TileSize))/TileSize;

	return FVector2d(TempLoc.X, TempLoc.Y);
}

FVector3d AGrid::ConvertIndexToLocation(FVector2d Index)
{
	return GridData.Find(Index)->TileTransform.GetLocation();
}
