// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "Unit_Child_Warrior.generated.h"

class AUpwall;



/**
 * 
 */
UCLASS()
class DIVINECONFLICT_API AUnit_Child_Warrior : public AUnit
{
	GENERATED_BODY()

	// UPROPERTIES
public:
	
protected:

	// UFUNCTIONS
public:
	// ----------------------------
	// --       Constructor      --
	// ----------------------------
	
	AUnit_Child_Warrior();
	
protected:
	// ----------------------------
	// --        Overrides       --
	// ----------------------------
	
	virtual void BeginPlay() override;

	virtual void DisplayWidgetTutorial() override;
	
};
