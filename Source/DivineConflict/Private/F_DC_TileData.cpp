// Fill out your copyright notice in the Description page of Project Settings.


#include "F_DC_TileData.h"

FDC_TileData::FDC_TileData()
{
	TilePosition.X = 0;
	TilePosition.Y = 0;
	TileType = EDC_TileType::None;
	TileTransform = FTransform3d();
	TileState = TArray<EDC_TileState>();
	UnitOnTile = nullptr;
	BuildingOnTile = nullptr;
	BaseOnTile = nullptr;
}

FDC_TileData::FDC_TileData(FIntPoint tilePosition, EDC_TileType tileType, FTransform3d tileTransform,
	TArray<EDC_TileState> tileState, AUnit* unitOnTile, ABuilding* BuildingOnTile, ABase* baseOnTile , ATower* towerOnTile, AUpwall* UpwallOnTile)
{
	this->TilePosition.X = tilePosition.X;
	this->TilePosition.Y = tilePosition.Y;
	this->TileType = tileType;
	this->TileTransform = tileTransform;
	this->TileState = tileState;
	this->UnitOnTile = unitOnTile;
	this->BuildingOnTile = BuildingOnTile;
	this->BaseOnTile = baseOnTile;
	this->TowerOnTile = towerOnTile;	
	this->UpwallOnTile = UpwallOnTile;	
}

FDC_TileData::~FDC_TileData()
{
}
