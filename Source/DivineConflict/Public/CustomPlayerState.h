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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actions")
	bool GotCentralBuilding = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actions")
	int TurnPassed = 0;
	
	UPROPERTY(EditAnywhere,Replicated, BlueprintReadWrite, Category = "Actions")
	int MaxActionPoints = 10 + (GotCentralBuilding * UKismetMathLibrary::Clamp(TurnPassed, 0, 5));

	UPROPERTY(EditAnywhere,Replicated, BlueprintReadWrite, Category = "Actions")
	int CurrentPA = 10;

	UPROPERTY(EditAnywhere,Replicated, BlueprintReadWrite, Category = "Ressources")
	int WoodPoints = 0;

	UPROPERTY(EditAnywhere,Replicated, BlueprintReadWrite, Category = "Ressources")
	int StonePoints = 0;

	UPROPERTY(EditAnywhere,Replicated, BlueprintReadWrite, Category = "Ressources")
	int GoldPoints = 0;
	
	UPROPERTY(EditAnywhere,Replicated, BlueprintReadWrite, Category = "RessourceGathering")
	int WoodBuildingOwned = 0;

	UPROPERTY(EditAnywhere,Replicated, BlueprintReadWrite, Category = "RessourceGathering")
	int StoneBuildingOwned = 0;
	
	UPROPERTY(EditAnywhere,Replicated, BlueprintReadWrite, Category = "RessourceGathering")
	int GoldBuildingOwned = 0;

	UPROPERTY(EditAnywhere,Replicated, BlueprintReadWrite, Category = "Units")
	int CurrentUnitCount = 0;

	UPROPERTY(EditAnywhere,Replicated, BlueprintReadWrite, Category = "Units")
	int MaxUnitCount = 10;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Ref", meta = (AllowPrivate = "true"))
	ACustomGameState* GameStateRef;

	UPROPERTY(EditAnywhere, NotBlueprintable, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	int LoadingTimer = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_bIsActiveTurn , Category = "Turn")
	bool bIsActiveTurn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_PlayerTeam, Category = "Turn")
	EPlayer PlayerTeam = EPlayer::P_Hell;

	UPROPERTY()
	bool bIsReadyToSiwtchTurn = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actions")
	bool bIsInTutorial = false;
	
	
private:
	virtual void BeginPlay() override;



	// UFUNCTIONS //
public:

	UPROPERTY(VisibleAnywhere, Replicated, Category = "Ref", meta = (AllowPrivate = "true"))
	ACustomPlayerController* PlayerControllerRef;
	
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void OnRep_bIsActiveTurn();

	UFUNCTION()
	ACustomPlayerController* GetPlayerCustomController();
	
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void OnRep_PlayerTeam();
	
	UFUNCTION(BlueprintCallable, Category = "Actions")
	int GetMaxActionPoints();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool IsCentralBuildingOurs();

	UFUNCTION(BlueprintCallable, Category = "Ressources")
	int GetWoodPoints();

	UFUNCTION(BlueprintCallable, Category = "Ressources")
	void ChangeWoodPoints(int WoodChange, bool Add);

	UFUNCTION(BlueprintCallable, Category = "Ressources")
	int GetStonePoints();

	UFUNCTION()
	bool GetIsReadyToSwitchTurn();

	UFUNCTION()
	void SetIsReadyToSwitchTurn(bool Ready);

	UFUNCTION()
	void UpdateUI();
	
	// true to add ; false to remove
	UFUNCTION(BlueprintCallable, Category = "Ressources")
	void ChangeStonePoints(int StoneChange, bool Add);

	UFUNCTION(BlueprintCallable, Category = "Ressources")
	int GetGoldPoints();

	UFUNCTION(BlueprintCallable, Category = "Ressources")
	void ChangeGoldPoints(int GoldChange, bool Add);

	UFUNCTION(BlueprintCallable, Category = "Units")
	int GetUnits();

	UFUNCTION(BlueprintCallable, Category = "Units")
	void SetUnits(int UnitNumber);

	UFUNCTION(BlueprintCallable, Category = "Turn")
	void NewTurnBegin();
	

	// GETTERS FOR UNITS SPAWN COSTS


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

	// Warrior
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
	int GetWarriorWoodCost();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
	int GetWarriorStoneCost();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
	int GetWarriorGoldCost();

	// Leader
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
	int GetLeaderWoodCost();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
	int GetLeaderStoneCost();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
	int GetLeaderGoldCost();


	// GETTERS FOR NUMBER OF UNITS
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
	int GetMageCount();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
	int GetTankCount();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
	int GetWarriorCount();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Units")
	int GetLeaderCount();
	

	// SETTERS FOR NUMBER OF UNITS
	UFUNCTION(BlueprintCallable, Category = "Units")
	void SetMageCount(int NewCount);
	UFUNCTION(BlueprintCallable, Category = "Units")
	void SetTankCount(int NewCount);
	UFUNCTION(BlueprintCallable, Category = "Units")
	void SetWarriorCount(int NewCount);
	UFUNCTION(BlueprintCallable, Category = "Units")
	void SetLeaderCount(int NewCount);

	UFUNCTION(BlueprintCallable, Category = "Units")
	void SetPopulation();

	
protected:

	// SPAWN COSTS FOR UNITS

	// Mage
	UPROPERTY()
	int MageWoodCost = 20;
	UPROPERTY()
	int MageStoneCost = 0;
	UPROPERTY()
	int MageGoldCost = 10;

	// Tank
	UPROPERTY()
	int TankWoodCost = 10;
	UPROPERTY()
	int TankStoneCost = 20;
	UPROPERTY()
	int TankGoldCost = 0;

	// Warrior
	UPROPERTY()
	int WarriorWoodCost = 0;
	UPROPERTY()
	int WarriorStoneCost = 10;
	UPROPERTY()
	int WarriorGoldCost = 20;


	// Leader
	UPROPERTY()
	int LeaderWoodCost = 30;
	UPROPERTY()
	int LeaderStoneCost = 30;
	UPROPERTY()
	int LeaderGoldCost = 30;

	// NUMBER OF EACH UNITS

	UPROPERTY(Replicated)
	int MageCount = 0;
	UPROPERTY(Replicated)
	int TankCount = 0;
	UPROPERTY(Replicated)
	int WarriorCount = 1;
	UPROPERTY(Replicated)
	int LeaderCount = 0;

};
