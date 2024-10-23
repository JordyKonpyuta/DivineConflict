// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "Unit_Child_Mage.generated.h"

/**
 * 
 */
UCLASS()
class DIVINECONFLICT_API AUnit_Child_Mage : public AUnit
{
	GENERATED_BODY()

	// UPROPERTIES
public:
	
protected:
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Stats")
	AUnit* UnitToAttack = nullptr;

	UPROPERTY(Replicated)
	ABase* BaseToAttack = nullptr;

	// UFUNCTIONS
public:
	// ----------------------------
	// Constructor
	AUnit_Child_Mage();
	
	// ----------------------------
	// Overrides

	virtual void Special() override;

	virtual void PrepareSpecial(FIntPoint SpecialPos) override;

	virtual void DisplayWidgetTutorial() override;

protected:
	// ----------------------------
	// Overrides
	
	virtual void BeginPlay() override;
};
