// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "Unit_Child_Leader.generated.h"
/**
 * 
 */
UCLASS()
class DIVINECONFLICT_API AUnit_Child_Leader : public AUnit
{
	GENERATED_BODY()

	// UPROPERTIES
public:

	UPROPERTY()
	TArray<AUnit*> AllUnitsToBuff;
	
protected:

	// UFUNCTIONS
public:
	// ----------------------------
	// Constructor
	
	AUnit_Child_Leader();

	// ----------------------------
	// Overrides
	
	virtual void Special() override;

	virtual void MoveUnitEndTurn() override;
	
	// ----------------------------
	// Buff

	void PushBuff();
	
protected:
	// ----------------------------
	// Overrides
	virtual void BeginPlay() override;
};
