// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPlayerState.h"


int ACustomPlayerState::GetActionPoints()
{
	return ActionPoints;
}

int ACustomPlayerState::GetMaxActionPoints()
{
	return MaxActionPoints;
}

bool ACustomPlayerState::IsCentralBuildingOurs()
{
	return GotCentralBuilding;
}

void ACustomPlayerState::RefreshActionPoints()
{
	ActionPoints = MaxActionPoints;
}