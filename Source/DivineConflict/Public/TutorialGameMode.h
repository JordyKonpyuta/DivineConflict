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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AUnit> Warrior1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AUnit> Warrior2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AUnit> Mage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AUnit> Leader;
	
protected:
	UPROPERTY()
	TObjectPtr<AGrid> Grid;

	// UFUNCTIONS
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DisplayTutorialWidget(int index);

	UFUNCTION(BlueprintNativeEvent)
	void BeginTutorial();

	UFUNCTION(BlueprintCallable)
	void HilightUnit(AUnit* UnitToHilight);

protected:
	virtual void BeginPlay() override;
};
