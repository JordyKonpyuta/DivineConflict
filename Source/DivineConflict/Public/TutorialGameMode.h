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

protected:
	virtual void BeginPlay() override;

	AGrid* Grid;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AUnit* Warrior1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AUnit* Warrior2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AUnit* Mage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AUnit* Leader;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DisplayTutorialWidget(int index);

	UFUNCTION(BlueprintNativeEvent)
	void BeginTutorial();

	UFUNCTION(BlueprintCallable)
	void HilightUnit(AUnit* UnitToHilight);
};
