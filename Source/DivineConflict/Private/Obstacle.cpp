// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle.h"

#include "Grid.h"
#include "Net/UnrealNetwork.h"


AObstacle::AObstacle()
{
 	PrimaryActorTick.bCanEverTick = true;
	
	// Set up appearance
	Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClimbPlane_Low"));
	Scene  = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));

	Plane->SetStaticMesh( ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'")).Object);
	
	Plane->SetupAttachment(Scene);

	Plane->SetRelativeLocation(FVector(0,0,1));
	
}

void AObstacle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AObstacle, Scene);
	DOREPLIFETIME(AObstacle, Plane);
}

void AObstacle::BeginPlay()
{
	Super::BeginPlay();
	
	Plane->SetVisibility(false);
}

void AObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

