// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "Unit_Child_Tank.generated.h"

/**
 * 
 */
UCLASS()
class DIVINECONFLICT_API AUnit_Child_Tank : public AUnit
{
	GENERATED_BODY()

	// UPROPERTIES
public:
	
protected:

	FTimerHandle TimerHandle;

	// UFUNCTIONS
public:
	// ----------------------------
	// Constructor
	
	AUnit_Child_Tank();


	// ----------------------------
	// Overrides
	
	virtual void DisplayWidgetTutorial() override;

private:
	// ----------------------------
	// Overrides
	
	virtual void BeginPlay() override;
};
