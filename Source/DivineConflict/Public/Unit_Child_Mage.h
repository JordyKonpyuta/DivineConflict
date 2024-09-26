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

protected:
	virtual void BeginPlay() override;

public:
	AUnit_Child_Mage();

	void SpecialUnit(AUnit* UnitToAttack) override;

	void SpecialBase(ABase* BaseToAttack) override;
	
};
