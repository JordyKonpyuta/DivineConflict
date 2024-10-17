// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbActor.h"


// ----------------------------
// Constructor
AClimbActor::AClimbActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	StaticMeshClimbable = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building Appearance"));
    AClimbActor::SetActorHiddenInGame(false);

}

// ----------------------------
// Overrides

void AClimbActor::BeginPlay()
{
	Super::BeginPlay();

	
}

void AClimbActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}