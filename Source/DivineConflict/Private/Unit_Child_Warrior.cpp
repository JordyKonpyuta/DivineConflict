// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit_Child_Warrior.h"

void AUnit_Child_Warrior::BeginPlay()
{
	Super::BeginPlay();

	SetAttack(12);
	SetDefense(4);
	SetMaxHealth(11);
	SetCurrentHealth(GetMaxHealth());
	SetPM(5);

	SetName(Names[FMath::RandRange(0, Names.Num()-1)]);
}
