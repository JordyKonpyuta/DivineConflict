// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "F_DC_TileData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FDC_GridData.generated.h"

/**
 * 
 */

class AUnit;
class ABuilding;
class ABase;
class ATower;


/*
UENUM(BlueprintType)
enum class EDC_TileType : uint8
{
	None UMETA(DisplayName = "None"),
	Normal UMETA(DisplayName = "Normal"),
	Obstacle UMETA(DisplayName = "Obstacle"),
	Climbable UMETA(DisplayName = "Climbable"),
	Building UMETA(DisplayName = "Building"),
	Gate UMETA(DisplayName = "Gate"),
	Tower UMETA(DisplayName = "Tower"),
	Walkable UMETA(DisplayName = "Walkable"),
};



UENUM(BlueprintType)
enum class EDC_TileState : uint8
{

	None UMETA(DisplayName = "None"),
	Hovered UMETA(DisplayName = "Hovered"),
	Selected UMETA(DisplayName = "Selected"),
	Reachable UMETA(DisplayName = "Reachable"),
	Pathfinding	UMETA(DisplayName = "Pathfinding"),
	Attacked UMETA(DisplayName = "Attacked"),
	Spawnable UMETA(DisplayName = "Spawnable"),
	NotSpawnable UMETA(DisplayName = "NotSpawnable"),

};*/

UCLASS(BlueprintType)
class DIVINECONFLICT_API UFDC_GridData : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};
/*
USTRUCT(BlueprintType)
struct FGRidData
{
	GENERATED_BODY()

public:

	TMap<FIntPoint,FDC_TileData> GridDateReplicted;
};*/

	

