// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Unit.generated.h"

UCLASS()
class DIVINECONFLICT_API AUnit : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AUnit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Units stats
	UPROPERTY()
	int Attack;
	
	UPROPERTY()
	int Defense;
	
	UPROPERTY()
	int CurrentHealth;
	
	UPROPERTY()
	int MaxHealth;
	
	UPROPERTY()
	int MovementCost;
	
	UPROPERTY()
	int AttackCost;
	
	UPROPERTY()
	int SpellCost;

	UPROPERTY()
	bool IsSelected = false;

	UPROPERTY()
	bool IsHell = false;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Getter for units stats
	int GetAttack();
	int GetDefense();
	int GetCurrentHealth();
	int GetMaxHealth();
	int GetMovementCost();
	int GetAttackCost();
	int GetSpellCost();
	bool GetIsSelected();
	bool GetIsHell();

	// Setter for units stats
	void SetAttack(int a);
	void SetDefense(int d);
	void SetCurrentHealth(int ch);
	void SetMaxHealth(int mh);
	void SetMovementCost(int mc);
	void SetAttackCost(int ac);
	void SetSpellCost(int sc);
	void SetIsSelected(bool s);
	void SetIsHell(bool h);

};
