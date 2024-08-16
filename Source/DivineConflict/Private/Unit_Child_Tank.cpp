// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit_Child_Tank.h"

void AUnit_Child_Tank::BeginPlay()
{
	Super::BeginPlay();

	SetAttack(10);
	SetDefense(9);
	SetMaxHealth(12);
	SetCurrentHealth(GetMaxHealth());
	SetPM(3);

	SetName(Names[FMath::RandRange(0, Names.Num()-1)]);
}
