// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"

// Sets default values
ATower::ATower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ATower::SetMesh_Implementation()
{
}

// Called when the game starts or when spawned
void ATower::BeginPlay()
{
	Super::BeginPlay();

	SetMesh();
	
}

// Called every frame
void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int ATower::GetAttack()
{
	return Attack;
}

void ATower::SetAttack(int NewAttack)
{
	Attack = NewAttack;
}

bool ATower::GetIsHell()
{
	return IsHell;
}

void ATower::SetIsHell(bool NewIsHell)
{
	IsHell = NewIsHell;
}

AUnit* ATower::GetUnitInGarnison()
{
	return UnitInGarnison;
}

void ATower::SetUnitInGarnison(AUnit* NewUnitInGarnison)
{
	UnitInGarnison = NewUnitInGarnison;
}

