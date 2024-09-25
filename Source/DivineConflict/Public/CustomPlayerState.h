// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/KismetMathLibrary.h"
#include "CustomPlayerState.generated.h"

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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actions")
	int MaxActionPoints = 10 + (GotCentralBuilding * UKismetMathLibrary::Clamp(TurnPassed, 0, 5));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ressources")
	int WoodPoints = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ressources")
	int StonePoints = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ressources")
	int GoldPoints = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RessourceGathering")
	int WoodBuildingOwned = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RessourceGathering")
	int StoneBuildingOwned = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RessourceGathering")
	int GoldBuildingOwned = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Units")
	int CurrentUnitCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Units")
	int MaxUnitCount = 10;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Ref", meta = (AllowPrivate = "true"))
	ACustomGameState* GameStateRef;

	UPROPERTY(EditAnywhere, NotBlueprintable, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	int LoadingTimer = 1;
	
private:
	virtual void BeginPlay() override;

	// UFUNCTIONS //
public:
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void RefreshActionPoints();

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
	
private:

};
