// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbActor.h"

// Sets default values
AClimbActor::AClimbActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	StaticMeshClimbable = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building Appearance"));
    AClimbActor::SetActorHiddenInGame(false);

}

// Called when the game starts or when spawned
void AClimbActor::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called every frame
void AClimbActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

