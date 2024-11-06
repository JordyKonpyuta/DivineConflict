// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumsList.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/KismetMathLibrary.h"
#include "CustomPlayerState.generated.h"

class ACustomPlayerController;
class ACustomGameState;

/**
 * 
 */
UCLASS()
class DIVINECONFLICT_API ACustomPlayerState : public APlayerState
{
	GENERATED_BODY()

	// UPROPERTIES //
public:
	// ----------------------------
	// References

	UPROPERTY(VisibleAnywhere, Replicated, Category = "Ref", meta = (AllowPrivate = "true"))
	TObjectPtr<ACustomPlayerController> PlayerControllerRef;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Ref", meta = (AllowPrivate = "true"))
	TObjectPtr<ACustomGameState> GameStateRef;
	
	// ----------------------------
	// Buildings
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actions")
	bool GotCentralBuilding = false;
	
	UPROPERTY(EditAnywhere,Replicated, BlueprintReadWrite, Category = "RessourceGathering")
	int WoodBuildingOwned = 0;

	UPROPERTY(EditAnywhere,Replicated, BlueprintReadWrite, Category = "RessourceGathering")
	int StoneBuildingOwned = 0;
	
	UPROPERTY(EditAnywhere,Replicated, BlueprintReadWrite, Category = "RessourceGathering")
	int GoldBuildingOwned = 0;
	
	// ----------------------------
	// Turns

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_PlayerTeam, Category = "Turn")
	EPlayer PlayerTeam = EPlayer::P_Hell;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actions")
	int TurnPassed = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_bIsActiveTurn , Category = "Turn")
	bool bIsActiveTurn = false;
	
	UPROPERTY()
	bool bIsReadyToSiwtchTurn = false;

	UPROPERTY(Replicated)
	bool bIsBlockTimerNewBeginTurn = false;
	
	// ----------------------------
	// Timers

	UPROPERTY(EditAnywhere, NotBlueprintable, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	int LoadingTimer = 1;
	
	// ----------------------------
	// Tutorial
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actions")
	bool bIsInTutorial = false;
	
	
protected:
	// ----------------------------
	// Ressources
	
	UPROPERTY(EditAnywhere,Replicated, BlueprintReadWrite, Category = "Actions")
	int MaxActionPoints = 10;

	UPROPERTY(EditAnywhere,Replicated, BlueprintReadWrite, Category = "Actions")
	int CurrentPA = 10;

	UPROPERTY(EditAnywhere,Replicated, BlueprintReadWrite, Category = "Ressources")
	int WoodPoints = 0;

	UPROPERTY(EditAnywhere,Replicated, BlueprintReadWrite, Category = "Ressources")
	int StonePoints = 0;

	UPROPERTY(EditAnywhere,Replicated, BlueprintReadWrite, Category = "Ressources")
	int GoldPoints = 0;
	
	// ----------------------------
	// UNITS
	
		// Warrior Costs
	UPROPERTY()
	int WarriorWoodCost = 0;
	UPROPERTY()
	int WarriorStoneCost = 10;
	UPROPERTY()
	int WarriorGoldCost = 20;
	
		// Mage Costs
	UPROPERTY()
	int MageWoodCost = 20;
	UPROPERTY()
	int MageStoneCost = 0;
	UPROPERTY()
	int MageGoldCost = 10;

		// Tank Costs
	UPROPERTY()
	int TankWoodCost = 10;
	UPROPERTY()
	int TankStoneCost = 20;
	UPROPERTY()
	int TankGoldCost = 0;
	
		// Leader Costs
	UPROPERTY()
	int LeaderWoodCost = 30;
	UPROPERTY()
	int LeaderStoneCost = 30;
	UPROPERTY()
	int LeaderGoldCost = 30;

		// Units Count
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Units")
	int MaxUnitCount = 10;
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Units")
	int CurrentUnitCount = 0;
	
	UPROPERTY(Replicated)
	int MageCount = 0;
	UPROPERTY(Replicated)
	int TankCount = 0;
	UPROPERTY(Replicated)
	int WarriorCount = 1;
	UPROPERTY(Replicated)
	int LeaderCount = 0;

	//---------------------------
	// Timer

	UPROPERTY()
	FTimerHandle BlockNewTurnbeginTimerHandle;
	
	// UFUNCTIONS //
public:
	
	// ----------------------------
	// On_Reps
	
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void OnRep_bIsActiveTurn();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void OnRep_PlayerTeam();
	
	// ----------------------------
	// Turns

	UFUNCTION(BlueprintCallable, Category = "Turn")
	void NewTurnBegin();

	UFUNCTION()
	void BlockNewTurnBegin();
	
	// ----------------------------
	// UI

	UFUNCTION()
	void UpdateUI();


	// ----------------------------
	// GETTERS

	// Refs
	
	UFUNCTION()
	ACustomPlayerController* GetPlayerCustomController();

	// Turns
	
	UFUNCTION()
	bool GetIsReadyToSwitchTurn();

	// Ressources
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Actions")
	int GetMaxActionPoints();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Actions")
	int GetActionPoints();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ressources")
	int GetWoodPoints();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ressources")
	int GetStonePoints();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ressources")
	int GetGoldPoints();

	// Units

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
	int GetUnits();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
	int GetMaxUnits();
	
		// GETTERS FOR UNITS SPAWN COSTS
		// Warrior
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
    int GetWarriorWoodCost();
	
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
    int GetWarriorStoneCost();
	
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
    int GetWarriorGoldCost();
    	
		// Mage
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
    int GetMageWoodCost();
	
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
    int GetMageStoneCost();
	
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
    int GetMageGoldCost();

		// Tank
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
    int GetTankWoodCost();
	
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
    int GetTankStoneCost();
	
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
    int GetTankGoldCost();
    
		// Leader
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
    int GetLeaderWoodCost();
	
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
    int GetLeaderStoneCost();
	
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
    int GetLeaderGoldCost();

	
		// GETTER FOR THE UNIT COUNTS
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
    int GetWarriorCount();
	
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
    int GetMageCount();
	
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
    int GetTankCount();
	
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
    int GetLeaderCount();
	
	// ----------------------------
	// SETTERS

	// Turns
	
	UFUNCTION()
	void SetIsReadyToSwitchTurn(bool Ready);

	// Units
	
	UFUNCTION(BlueprintCallable, Category = "Units")
	void SetUnits(int UnitNumber);
	
	// true to add ; false to remove
	UFUNCTION(BlueprintCallable, Category = "Units")
	void SetMaxUnits(int UnitMax);

	// SETTERS FOR RESSOURCES
	
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void SetMaxActionPoints(int MaxAP);
	
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void SetActionPoints(int CurAP);
	
	UFUNCTION(BlueprintCallable, Category = "Ressources")
	void ChangeWoodPoints(int WoodChange, bool Add);
	
	UFUNCTION(BlueprintCallable, Category = "Ressources")
	void ChangeStonePoints(int StoneChange, bool Add);

	UFUNCTION(BlueprintCallable, Category = "Ressources")
	void ChangeGoldPoints(int GoldChange, bool Add);
	
	// SETTERS FOR UNIT COUNTS
	
	UFUNCTION(BlueprintCallable, Category = "Units")
	void SetWarriorCount(int NewCount);
	
	UFUNCTION(BlueprintCallable, Category = "Units")
	void SetMageCount(int NewCount);
	
	UFUNCTION(BlueprintCallable, Category = "Units")
	void SetTankCount(int NewCount);
	
	UFUNCTION(BlueprintCallable, Category = "Units")
	void SetLeaderCount(int NewCount);

	UFUNCTION(BlueprintCallable, Category = "Units")
	void SetPopulation();
	
protected:
	// ----------------------------
	// Overrides

	virtual void BeginPlay() override;
};
