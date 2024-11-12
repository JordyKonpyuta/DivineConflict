// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DC_SaveGameSystem.generated.h"

/**
 * 
 */

class AUnit;
class ABuilding;

UCLASS()
class DIVINECONFLICT_API UDC_SaveGameSystem : public USaveGame
{
	GENERATED_BODY()

public:

	// GENERAL
	UPROPERTY()
	FString SaveSlotName = "Global";

	// PLAYER INFOS
	UPROPERTY()
	FVector PlayerLocation = FVector(0,0,0);

	UPROPERTY()
	FRotator PlayerRotation = FRotator(0,0,0);

	UPROPERTY()
	int CurrentUnits = 0;

	UPROPERTY()
	int MaxUnits = 0;

	// TURN INFOS
	UPROPERTY()
	bool bIsInActiveTurn = true;

	UPROPERTY()
	int CurrentTurn = 0;

	// PLAYER RESSOURCES
	UPROPERTY()
	int PlayerGold = 0;

	UPROPERTY()
	int PlayerWood = 0;

	UPROPERTY()
	int PlayerStone	= 0;

	// UNITS
	UPROPERTY()
	TArray<AUnit*> HeavenUnits;
	UPROPERTY()
	int HeavenUnitsCount = 0;

	UPROPERTY()
	TArray<AUnit*> HellUnits;
	UPROPERTY()
	int HellUnitsCount = 0;

	UPROPERTY()
	TArray<AUnit*> NeutralUnits;
	UPROPERTY()
	int NeutralUnitsCount = 0;

	// BUILDS
	UPROPERTY()
	TArray<ABuilding*> Builds;
	
};
