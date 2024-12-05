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
	// --        Component       --
	// ----------------------------
	
	UPROPERTY()
	TObjectPtr<AProjectile> FireBall;
	
protected:

	// UFUNCTIONS
public:
	// ----------------------------
	// --       Constructor      --
	// ----------------------------
	AUnit_Child_Mage();
	
	// ----------------------------
	// --        Override        --
	// ----------------------------

	// Widget
	
	virtual void DisplayWidgetTutorial() override;

	// Attacks
	
	virtual void AttackUnit(AUnit* UnitToAAttack, bool bInitiateAttack) override;

	virtual void AttackBase(ABase* BaseToAAttack) override;

	virtual void AttackBuilding(ABuilding* BuildingToAttack) override;

protected:
	
	// ----------------------------
	// --        Override        --
	// ----------------------------
	
	virtual void BeginPlay() override;
	
	// ----------------------------
	// --           FX           --
	// ----------------------------

	UFUNCTION()
	void GenerateFX(AActor* Target);
};
