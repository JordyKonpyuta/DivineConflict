// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPlayerState.h"
#include "CustomGameState.h"
#include "CustomPlayerController.h"
#include "Net/UnrealNetwork.h"


void ACustomPlayerState::BeginPlay()
{
	Super::BeginPlay();

	GameStateRef = GetWorld()->GetGameState<ACustomGameState>();

	GameStateRef->AllPlayerStates.Add(this);
	
}

void ACustomPlayerState::OnRep_bIsActiveTurn()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OnRep_bIsActiveTurn : " + FString::FromInt(bIsActiveTurn)));
	if(bIsActiveTurn)
	{
		NewTurnBegin();
	}
}

void ACustomPlayerState::OnRep_PlayerTeam()
{

}

void ACustomPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
	DOREPLIFETIME(ACustomPlayerState, bIsActiveTurn);
	DOREPLIFETIME(ACustomPlayerState, PlayerTeam);
	DOREPLIFETIME(ACustomPlayerState, MaxActionPoints);
	DOREPLIFETIME(ACustomPlayerState, CurrentPA);
	DOREPLIFETIME(ACustomPlayerState, WoodPoints);
	DOREPLIFETIME(ACustomPlayerState, StonePoints);
	DOREPLIFETIME(ACustomPlayerState, GoldPoints);
	DOREPLIFETIME(ACustomPlayerState, WoodBuildingOwned);
	DOREPLIFETIME(ACustomPlayerState, StoneBuildingOwned);
	DOREPLIFETIME(ACustomPlayerState, GoldBuildingOwned);
	DOREPLIFETIME(ACustomPlayerState, CurrentUnitCount);
	DOREPLIFETIME(ACustomPlayerState, MaxUnitCount);

}

int ACustomPlayerState::GetMaxActionPoints()
{
	return MaxActionPoints;
}

bool ACustomPlayerState::IsCentralBuildingOurs()
{
	return GotCentralBuilding;
}

int ACustomPlayerState::GetWoodPoints()
{
	return WoodPoints;
}

int ACustomPlayerState::GetStonePoints()
{
	return StonePoints;
}

int ACustomPlayerState::GetGoldPoints()
{
	return GoldPoints;
}

int ACustomPlayerState::GetUnits()
{
	return CurrentUnitCount;
}

void ACustomPlayerState::ChangeWoodPoints(int WoodChange, bool Add)
{
	if(Add == true)
	{
		WoodPoints += WoodChange;
	}
	else
	{
		WoodPoints -= WoodChange;
	}
}

void ACustomPlayerState::ChangeStonePoints(int StoneChange, bool Add)
{
	if(Add == true)
	{
		StonePoints += StoneChange;
	}
	else
	{
		StonePoints -= StoneChange;
	}
}

void ACustomPlayerState::ChangeGoldPoints(int GoldChange, bool Add)
{
	if(Add == true)
	{
		GoldPoints += GoldChange;
	}
	else
	{
		GoldPoints -= GoldChange;
	}
}

void ACustomPlayerState::SetUnits(int UnitNumber)
{
	CurrentUnitCount = UnitNumber;
}

void ACustomPlayerState::NewTurnBegin()
{
	ChangeWoodPoints(5 + (WoodBuildingOwned * 15), true);
	ChangeStonePoints(4 + (StoneBuildingOwned * 15), true);
	ChangeGoldPoints(2 + (GoldBuildingOwned * 15), true);

	if (PlayerControllerRef)
	{
		CurrentPA = 10 + (GotCentralBuilding * UKismetMathLibrary::Clamp(TurnPassed, 0, 5));
		PlayerControllerRef->CurrentPA = CurrentPA;
	}
}
