// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner.h"
#include "Grid.h"
#include "GridInfo.h"

// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spawner Mesh"));
	SetRootComponent(SpawnerMesh);
	

}

// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
	Super::BeginPlay();

	if(Grid != nullptr)
	{
		Grid->GridInfo->addSpawnUnitOnGrid(Grid->ConvertLocationToIndex(GetActorLocation()), this);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Spawner on tile %s and this position is %d %d"), *GetName(), GridPosition.X, GridPosition.Y));
	}
	
}

// Called every frame
void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpawner::SetGridPosition(FIntVector2 Position)
{
	GridPosition = Position;
}

FIntVector2 ASpawner::GetGridPosition()
{
	return GridPosition;
}

