// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"


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

	SpawnGrid();
	
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
				FIntVector2 TileIndex = FIntVector2(X, Y);
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

// Called every frame
void AGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	
}
