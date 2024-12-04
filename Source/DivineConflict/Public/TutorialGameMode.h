// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TutorialGameMode.generated.h"

/**
 * 
 */

class AGrid;
class AUnit;

UCLASS()
class DIVINECONFLICT_API ATutorialGameMode : public AGameModeBase
{
	GENERATED_BODY()

	// UPROPERTIES
public:
	// ----------------------------
	// --       References       --
	// ----------------------------

	// Units
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AUnit> Tank = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AUnit> Warrior1 = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AUnit> Warrior2 = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AUnit> Mage = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AUnit> Leader = nullptr;

	// Buildings

	// Bases

	
	// ----------------------------
	// --       Unit Values      --
	// ----------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int UnitProduced = 0;
	
	// ----------------------------
	// --        Booleans        --
	// ----------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDead = false;
	
protected:
	// ----------------------
	// --	 References	   --
	// ----------------------
	
	UPROPERTY()
	TObjectPtr<AGrid> Grid = nullptr;

	// UFUNCTIONS
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DisplayTutorialWidget(int index);

	UFUNCTION(BlueprintNativeEvent)
	void BeginTutorial();

	UFUNCTION(BlueprintCallable)
	void HighlightUnit(AUnit* UnitToHighlight);

protected:
	virtual void BeginPlay() override;
};
