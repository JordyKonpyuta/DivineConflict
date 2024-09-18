// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>
#include "F_DC_TileData.generated.h"

/**
 * 
 */

class AUnit;
class ASpawner;

enum class DIVINECONFLICT_API E_DC_TileTypp
{
	None,
	Normal,
	Obstacle,
	Gate,
	Tower,
	Walkable,
};

enum class DIVINECONFLICT_API E_DC_TileState
{
	None,
	Hovered,
	Selected,
	Reachable,
	Pathfinding,
	Attacked
};



USTRUCT(BlueprintType)
struct DIVINECONFLICT_API FDC_TileData
{
	GENERATED_BODY()
public:
	FDC_TileData();
	FDC_TileData(FIntPoint tilePosition, E_DC_TileTypp tileType, FTransform3d tileTransform,
	             TArray<E_DC_TileState> tileState, AUnit* unitOnTile, ASpawner* spawnerOnTile);
	
	
	~FDC_TileData();
	UPROPERTY( EditAnywhere, Category = "GridElement")
	FIntPoint TilePosition;
	
	E_DC_TileTypp TileType;
	UPROPERTY( EditAnywhere, Category = "GridElement")
	FTransform3d TileTransform;
	
	TArray<E_DC_TileState> TileState;
	UPROPERTY( EditAnywhere, Category = "GridElement")
	AUnit* UnitOnTile = nullptr;
	UPROPERTY( EditAnywhere, Category = "GridElement")
	ASpawner* SpawnerOnTile = nullptr;
};
