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
	
	// ----------------------------
	// Special

	bool bIsUsingSpecial = false;

	// UFUNCTIONS
public:
	// ----------------------------
	// Constructor
	
	AUnit_Child_Tank();
	
	// ----------------------------
	// Overrides

	void Special() override;
	
	// ----------------------------
	// GETTERS

	bool GetIsUsingSpecial();
	
	// ----------------------------
	// SETTERS

	void SetIsUsingSpecial(bool bIsBoosting);

private:
	// ----------------------------
	// Overrides
	
	virtual void BeginPlay() override;
};
