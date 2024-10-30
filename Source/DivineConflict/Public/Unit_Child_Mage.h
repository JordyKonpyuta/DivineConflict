// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "Unit_Child_Mage.generated.h"

class AProjectile;
/**
 * 
 */
UCLASS()
class DIVINECONFLICT_API AUnit_Child_Mage : public AUnit
{
	GENERATED_BODY()

	// UPROPERTIES
public:
	// ----------------------------
	// Component
	
	UPROPERTY()
	TObjectPtr<AProjectile> FireBall;
	
protected:
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Stats")
	TObjectPtr<AUnit> UnitToAttack = nullptr;

	UPROPERTY(Replicated)
	TObjectPtr<ABase> BaseToAttack = nullptr;

	// UFUNCTIONS
public:
	// ----------------------------
	// Constructor
	AUnit_Child_Mage();
	
	// ----------------------------
	// Overrides

	void SpecialMage(AActor* Target);

	virtual void DisplayWidgetTutorial() override;

protected:
	// ----------------------------
	// Overrides
	
	virtual void BeginPlay() override;
};
