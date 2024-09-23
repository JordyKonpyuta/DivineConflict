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



UENUM(BlueprintType)
enum class EDC_TileType : uint8
{
	None UMETA(DisplayName = "None"),
	Normal UMETA(DisplayName = "Normal"),
	Obstacle UMETA(DisplayName = "Obstacle"),
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



};



USTRUCT(BlueprintType)
struct DIVINECONFLICT_API FDC_TileData
{
	GENERATED_BODY()
public:
	FDC_TileData();
	FDC_TileData(FIntPoint tilePosition, EDC_TileType tileType, FTransform3d tileTransform,
	             TArray<EDC_TileState> tileState, AUnit* unitOnTile, ABuilding* spawnerOnTile, ABase* baseOnTile);
	
	
	~FDC_TileData();
	
	UPROPERTY( EditAnywhere, Category = "GridElement")
	FIntPoint TilePosition;
	
	UPROPERTY(EditAnywhere, Category = "GridElement")
	EDC_TileType TileType;
	
	UPROPERTY( EditAnywhere, Category = "GridElement")
	FTransform3d TileTransform;

	UPROPERTY( EditAnywhere, Category = "GridElement")
	TArray<EDC_TileState> TileState;
	
	UPROPERTY( EditAnywhere, Category = "GridElement")
	AUnit* UnitOnTile = nullptr;
	
	UPROPERTY( EditAnywhere, Category = "GridElement")
	ABuilding* SpawnerOnTile = nullptr;

	UPROPERTY( EditAnywhere, Category = "GridElement")
	ABase *BaseOnTile = nullptr;
};
