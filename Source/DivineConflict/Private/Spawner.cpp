// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner.h"


	// ----------------------------
	// Contructor

ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spawner Mesh"));
	SetRootComponent(SpawnerMesh);
	

}

	// ----------------------------
	// Overrides

void ASpawner::BeginPlay()
{
	Super::BeginPlay();

	if(Grid != nullptr)
	{
		
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Spawner on tile %s and this position is %d %d"), *GetName(), GridPosition.X, GridPosition.Y));
	}
	
}

void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

	// ----------------------------
	// Setter

void ASpawner::SetGridPosition(FIntPoint Position)
{
	GridPosition = Position;
}

	// ----------------------------
	// Getter

FIntPoint ASpawner::GetGridPosition()
{
	return GridPosition;
}