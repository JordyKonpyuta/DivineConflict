// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit_Child_Leader.h"


void AUnit_Child_Leader::BeginPlay()
{
	Super::BeginPlay();

	SetAttack(7);
	SetDefense(0);
	SetMaxHealth(20);
	SetCurrentHealth(GetMaxHealth());
	SetPM(3);

	SetName(Names[FMath::RandRange(0, Names.Num()-1)]);
}
