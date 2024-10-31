// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPlayerState.h"
#include "CustomGameState.h"
#include "CustomPlayerController.h"
#include "Net/UnrealNetwork.h"


	// ----------------------------
	// Overrides

void ACustomPlayerState::BeginPlay()
{
	Super::BeginPlay();

	GameStateRef = GetWorld()->GetGameState<ACustomGameState>();

	GameStateRef->AllPlayerStates.Add(this);

	SetPopulation();

	if (bIsInTutorial)
	{
		GoldPoints = 20;
		StonePoints = 20;
		WoodPoints = 20;
	}
	
}
	
	// ----------------------------
	// Replication (Variables)

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

	// ----------------------------
	// Replication (Functions)

void ACustomPlayerState::OnRep_bIsActiveTurn()
{
	
	if(!bIsActiveTurn)
	{
		NewTurnBegin();
	}
	UpdateUI();
	bIsReadyToSiwtchTurn = false;
	if (GameStateRef)
	{
		MaxActionPoints = 10 + GotCentralBuilding * UKismetMathLibrary::Clamp(1 + UKismetMathLibrary::FTrunc(GameStateRef->Turn / 5), 1, MAX_int32);
		CurrentPA = MaxActionPoints;
	}
	
}

void ACustomPlayerState::OnRep_PlayerTeam()
{
	if(Cast<ACustomPlayerController>(GetPlayerController()))
        Cast<ACustomPlayerController>(GetPlayerController())->SetPlayerTeam(PlayerTeam);
}

	// ----------------------------
	// Turns

void ACustomPlayerState::NewTurnBegin()
{
	ChangeWoodPoints(20 + (WoodBuildingOwned * 15), true);
	ChangeStonePoints(20 + (StoneBuildingOwned * 15), true);
	ChangeGoldPoints(20 + (GoldBuildingOwned * 15), true);
}
	
	// ----------------------------
	// UI

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
	
	// ----------------------------
	// GETTERS

		// Refs

ACustomPlayerController* ACustomPlayerState::GetPlayerCustomController()
{
	return Cast<ACustomPlayerController>(GetPlayerController());
}

		// Turns

bool ACustomPlayerState::GetIsReadyToSwitchTurn()
{
	return bIsReadyToSiwtchTurn;
}

		// Ressources

int ACustomPlayerState::GetMaxActionPoints()
{                            	
	return MaxActionPoints;
}

int ACustomPlayerState::GetActionPoints()
{
	return CurrentPA;
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

		// Units

int ACustomPlayerState::GetUnits()
{
	return CurrentUnitCount;
}

int ACustomPlayerState::GetMaxUnits()
{
	return MaxUnitCount;
}

// GETTERS FOR UNITS SPAWN COSTS
			// WARRIORS

int ACustomPlayerState::GetWarriorWoodCost()
{
	return WarriorWoodCost + (WarriorWoodCost*(0.25*GetWarriorCount()));
}

int ACustomPlayerState::GetWarriorStoneCost()
{
	return WarriorStoneCost + (WarriorStoneCost*(0.25*GetWarriorCount()));
}

int ACustomPlayerState::GetWarriorGoldCost()
{
	return WarriorGoldCost + (WarriorGoldCost*(0.25*GetWarriorCount()));
}

			// MAGES

int ACustomPlayerState::GetMageWoodCost()
{
	return MageWoodCost+(MageWoodCost*(0.25*GetMageCount()));
}

int ACustomPlayerState::GetMageStoneCost()
{
	return MageStoneCost + (MageStoneCost*(0.25*GetMageCount()));
}

int ACustomPlayerState::GetMageGoldCost()
{
	return MageGoldCost + (MageGoldCost*(0.25*GetMageCount()));
}

			// TANKS

int ACustomPlayerState::GetTankWoodCost()
{
	return TankWoodCost + (TankWoodCost*(0.25*GetTankCount()));
}

int ACustomPlayerState::GetTankStoneCost()
{
	return TankStoneCost + (TankStoneCost*(0.25*GetTankCount()));
}

int ACustomPlayerState::GetTankGoldCost()
{
	return TankGoldCost + (TankGoldCost*(0.25*GetTankCount()));
}

			// LEADERS

int ACustomPlayerState::GetLeaderWoodCost()
{
	return LeaderWoodCost + (LeaderWoodCost*(0.25*GetLeaderCount()));
}

int ACustomPlayerState::GetLeaderStoneCost()
{
	return LeaderStoneCost + (LeaderStoneCost*(0.25*GetLeaderCount()));
}

int ACustomPlayerState::GetLeaderGoldCost()
{
	return LeaderGoldCost + (LeaderGoldCost*(0.25*GetLeaderCount()));
}

		// GETTER FOR THE UNIT COUNTS

int ACustomPlayerState::GetWarriorCount()
{
	return WarriorCount;
}

int ACustomPlayerState::GetMageCount()
{
	return MageCount;
}

int ACustomPlayerState::GetTankCount()
{
	return TankCount;
}

int ACustomPlayerState::GetLeaderCount()
{
	return LeaderCount;
}


	// ----------------------------
	// SETTERS

		// Turns

void ACustomPlayerState::SetIsReadyToSwitchTurn(bool Ready)
{
	bIsReadyToSiwtchTurn = Ready;
}

// Units

void ACustomPlayerState::SetUnits(int UnitNumber)
{
	CurrentUnitCount = UnitNumber;
}

void ACustomPlayerState::SetMaxUnits(int UnitMax)
{
	MaxUnitCount = UnitMax;
}

// SETTERS FOR RESSOURCES

void ACustomPlayerState::SetMaxActionPoints(int MaxAP)
{
	MaxActionPoints = MaxAP;
}

void ACustomPlayerState::SetActionPoints(int CurAP)
{
	CurrentPA = CurAP;
	if (PlayerControllerRef)
		PlayerControllerRef->UpdateUi();
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

		// SETTERS FOR UNIT COUNTS

void ACustomPlayerState::SetWarriorCount(int NewCount)
{

	WarriorCount = NewCount;
	SetPopulation();
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

void ACustomPlayerState::SetLeaderCount(int NewCount)
{
	LeaderCount = NewCount;
	SetPopulation();
}

void ACustomPlayerState::SetPopulation()
{
	CurrentUnitCount = MageCount + TankCount + WarriorCount + LeaderCount;
}
