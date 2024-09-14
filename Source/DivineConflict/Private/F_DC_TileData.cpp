// Fill out your copyright notice in the Description page of Project Settings.


#include "F_DC_TileData.h"

FDC_TileData::FDC_TileData()
{
	TilePosition.X = 0;
	TilePosition.Y = 0;
	TileType = E_DC_TileTypp::None;
	TileTransform = FTransform3d();
	TileState = TArray<E_DC_TileState>();
	UnitOnTile = nullptr;
	SpawnerOnTile = nullptr;
}

FDC_TileData::FDC_TileData(FIntVector2 tilePosition, E_DC_TileTypp tileType, FTransform3d tileTransform,
	TArray<E_DC_TileState> tileState, AUnit* unitOnTile, ASpawner* spawnerOnTile)
{
	this->TilePosition.X = tilePosition.X;
	this->TilePosition.Y = tilePosition.Y;
	this->TileType = tileType;
	this->TileTransform = tileTransform;
	this->TileState = tileState;
	this->UnitOnTile = unitOnTile;
	this->SpawnerOnTile = spawnerOnTile;
}

FDC_TileData::~FDC_TileData()
{
}
