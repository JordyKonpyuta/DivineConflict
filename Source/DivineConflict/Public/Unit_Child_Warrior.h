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
	UPROPERTY(Replicated)
	TObjectPtr<AUpwall> WallToClimb;
	
protected:

	// UFUNCTIONS
public:
	// ----------------------------
	// Constructor
	
	AUnit_Child_Warrior();
	
	// ----------------------------
	// Overrides

	virtual void Special() override;

	// ----------------------------
	// Climbing

	UFUNCTION(Server, Reliable)
	void Server_MoveToClimb();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_MoveToClimb();

	UFUNCTION(Server,Reliable)
	void Server_SpecialMove(FIntPoint NewPos);

	UFUNCTION(NetMulticast,Reliable)
	void Multi_SpecialMove(FIntPoint NewPos);
	
protected:
	// ----------------------------
	// Overrides
	
	virtual void BeginPlay() override;

	virtual void DisplayWidgetTutorial() override;
	
};
