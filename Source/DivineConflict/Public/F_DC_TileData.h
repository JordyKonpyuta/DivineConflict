// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "F_DC_TileData.generated.h"

/**
 * 
 */

class AUnit;
class ABuilding;
class ABase;
class ATower;
class AUpwall;



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
	Spawned UMETA(DisplayName = "Spawned"),

};



USTRUCT(BlueprintType)
struct DIVINECONFLICT_API FDC_TileData
{
	GENERATED_BODY()
public:
	FDC_TileData();
	FDC_TileData(FIntPoint tilePosition, EDC_TileType tileType, FTransform3d tileTransform,
	             TArray<EDC_TileState> tileState, AUnit* unitOnTile, ABuilding* BuildingOnTile, ABase* baseOnTile , ATower* towerOnTile, AUpwall* UpwallOnTile);
	
	
	~FDC_TileData();
	
	UPROPERTY( EditAnywhere, Category = "GridElement")
	FIntPoint TilePosition;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GridElement")
	EDC_TileType TileType;
	
	UPROPERTY( EditAnywhere, Category = "GridElement")
	FTransform3d TileTransform;

	UPROPERTY( EditAnywhere, Category = "GridElement")
	TArray<EDC_TileState> TileState;
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "GridElement")
	TObjectPtr<AUnit> UnitOnTile = nullptr;
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "GridElement")
	TObjectPtr<ABuilding> BuildingOnTile = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "GridElement")
	TObjectPtr<ABase> BaseOnTile = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridElement")
	TObjectPtr<ATower> TowerOnTile = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridElement")
	TObjectPtr<AUpwall> UpwallOnTile = nullptr;
};

USTRUCT(BlueprintType)
struct FGRidData
{
	GENERATED_BODY()
	TMap<FIntPoint, FDC_TileData> GridDateReplicted;
};
