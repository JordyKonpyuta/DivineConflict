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

	if (bIsInTutorial)
	{
		GoldPoints = 20;
		StonePoints = 20;
		WoodPoints = 20;
	}
	
}

void ACustomPlayerState::OnRep_bIsActiveTurn()
{
	
	if(!bIsActiveTurn)
	{
		NewTurnBegin();
	}
	UpdateUI();
	bIsReadyToSiwtchTurn = false;
	
}

ACustomPlayerController* ACustomPlayerState::GetPlayerCustomController()
{
	return Cast<ACustomPlayerController>(GetPlayerController());
}

void ACustomPlayerState::OnRep_PlayerTeam()
{
	if(Cast<ACustomPlayerController>(GetPlayerController()))
        Cast<ACustomPlayerController>(GetPlayerController())->SetPlayerTeam(PlayerTeam);
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
	DOREPLIFETIME(ACustomPlayerState, PlayerControllerRef);
	DOREPLIFETIME(ACustomPlayerState, WarriorCount);
	DOREPLIFETIME(ACustomPlayerState, MageCount);
	DOREPLIFETIME(ACustomPlayerState, TankCount);
	DOREPLIFETIME(ACustomPlayerState, LeaderCount);
	

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

bool ACustomPlayerState::GetIsReadyToSwitchTurn()
{
	return bIsReadyToSiwtchTurn;
}

void ACustomPlayerState::SetIsReadyToSwitchTurn(bool Ready)
{
	bIsReadyToSiwtchTurn = Ready;
}

void ACustomPlayerState::UpdateUI()
{

	if(!PlayerControllerRef)
		PlayerControllerRef = Cast<ACustomPlayerController>(GetPlayerController());
	UE_LOG( LogTemp, Warning, TEXT("PlayerControllerRef"));
	if(PlayerControllerRef)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player %d is active turn"), PlayerTeam));
		PlayerControllerRef->UpdateUi();
		PlayerControllerRef->UpdateUITimer(90);
	}
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
	ChangeWoodPoints(20 + (WoodBuildingOwned * 15), true);
	ChangeStonePoints(20 + (StoneBuildingOwned * 15), true);
	ChangeGoldPoints(20 + (GoldBuildingOwned * 15), true);


		MaxActionPoints = 10 + (GotCentralBuilding * UKismetMathLibrary::Clamp(TurnPassed, 0, 5));
		CurrentPA = MaxActionPoints;
		//PlayerControllerRef->CurrentPA = CurrentPA;

}

int ACustomPlayerState::GetMageWoodCost()
{
	return MageWoodCost;
}

int ACustomPlayerState::GetMageStoneCost()
{
	return MageStoneCost;
}

int ACustomPlayerState::GetMageGoldCost()
{
	return MageGoldCost;
}

int ACustomPlayerState::GetTankWoodCost()
{
	return TankWoodCost;
}

int ACustomPlayerState::GetTankStoneCost()
{
	return TankStoneCost;
}

int ACustomPlayerState::GetTankGoldCost()
{
	return TankGoldCost;
}

int ACustomPlayerState::GetWarriorWoodCost()
{
	return WarriorWoodCost;
}

int ACustomPlayerState::GetWarriorStoneCost()
{
	return WarriorStoneCost;
}

int ACustomPlayerState::GetWarriorGoldCost()
{
	return WarriorGoldCost;
}

int ACustomPlayerState::GetLeaderWoodCost()
{
	return LeaderWoodCost;
}

int ACustomPlayerState::GetLeaderStoneCost()
{
	return LeaderStoneCost;
}

int ACustomPlayerState::GetLeaderGoldCost()
{
	return LeaderGoldCost;
}

int ACustomPlayerState::GetMageCount()
{
	return MageCount;
}

int ACustomPlayerState::GetTankCount()
{
	return TankCount;
}

int ACustomPlayerState::GetWarriorCount()
{
	return WarriorCount;
}

int ACustomPlayerState::GetLeaderCount()
{
	return LeaderCount;
}

void ACustomPlayerState::SetMageCount(int NewCount)
{
	MageCount = NewCount;
	SetPopulation();
}

void ACustomPlayerState::SetTankCount(int NewCount)
{
	TankCount = NewCount;
	SetPopulation();
}

void ACustomPlayerState::SetWarriorCount(int NewCount)
{

	WarriorCount = NewCount;
	SetPopulation();
}

void ACustomPlayerState::SetLeaderCount(int NewCount)
{
	LeaderCount = NewCount;
	SetPopulation();
}

void ACustomPlayerState::SetPopulation()
{
	CurrentUnitCount = MageCount + TankCount + WarriorCount + LeaderCount;
}
