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



USTRUCT()
struct DIVINECONFLICT_API FDC_TileData
{
	GENERATED_BODY()
public:
	FDC_TileData();
	FDC_TileData(FVector2D tilePosition, E_DC_TileTypp tileType, FTransform3d tileTransform,
	             std::vector<E_DC_TileState> tileState, AUnit* unitOnTile, ASpawner* spawnerOnTile);
	
	
	~FDC_TileData();

	FVector2D TilePosition;
	E_DC_TileTypp TileType;
	FTransform3d TileTransform;
	std::vector<E_DC_TileState> TileState;
	AUnit* UnitOnTile = nullptr;
	ASpawner* SpawnerOnTile = nullptr;
};
