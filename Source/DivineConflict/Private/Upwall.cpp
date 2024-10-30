// Fill out your copyright notice in the Description page of Project Settings.


#include "Upwall.h"

#include "Grid.h"
#include "GridInfo.h"
#include "Net/UnrealNetwork.h"

// ----------------------------
	// Constructor

AUpwall::AUpwall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set up appearance
	Plane1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClimbPlane_Low"));
	Plane2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClimbPlane_High"));
	Scene  = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));

	Plane1->SetStaticMesh( ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'")).Object);
	Plane2->SetStaticMesh( ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'")).Object);
	
	Plane2->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	Plane1->SetupAttachment(Scene);
	Plane2->SetupAttachment(Scene);

	Plane1->SetRelativeLocation(FVector(0,0,1));
	Plane2->SetRelativeLocation(FVector(0,100,101));
}

void AUpwall::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUpwall, Scene);
	DOREPLIFETIME(AUpwall, Plane1);
	DOREPLIFETIME(AUpwall, Plane2);
	DOREPLIFETIME(AUpwall, GridPosition);
	DOREPLIFETIME(AUpwall, ClimbLocation);
}
	// ----------------------------
	// Overrides

void AUpwall::BeginPlay()
{
	Super::BeginPlay();

	if (GridRef)
	{
		GridRef->GridInfo->AddClimbableOnGrid(GridRef->ConvertLocationToIndex(GetActorLocation()),this);

		ClimbLocation = GridRef->ConvertLocationToIndex(Plane2->GetComponentLocation());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("ClimbLocation : %s"), *ClimbLocation.ToString()));
		Plane1->SetVisibility(false);
		Plane2->SetVisibility(false);
	}
}

void AUpwall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

	// ----------------------------
	// GETTERS

FIntPoint AUpwall::GetGridPosition()
{
	return GridPosition;
}

FIntPoint AUpwall::GetClimbLocation()
{
	return ClimbLocation;
}

// ----------------------------
	// SETTERS

void AUpwall::SetGridPosition(FIntPoint NewGridPosition)
{
	GridPosition = NewGridPosition;
}

void AUpwall::SetClimbLocation(FIntPoint NewClimbLocation)
{
	ClimbLocation = NewClimbLocation;
}