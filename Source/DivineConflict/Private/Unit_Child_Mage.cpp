// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit_Child_Mage.h"

void AUnit_Child_Mage::BeginPlay()
{
	Super::BeginPlay();

	SetAttack(5);
	SetDefense(2);
	SetMaxHealth(10);
	SetCurrentHealth(GetMaxHealth());
	SetPM(4);
	
	SetName(Names[FMath::RandRange(0, Names.Num()-1)]);
}
