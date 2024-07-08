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
	UFUNCTION(BlueprintCallable)
	int GetAttack();

	UFUNCTION(BlueprintCallable)
	int GetDefense();

	UFUNCTION(BlueprintCallable)
	int GetCurrentHealth();

	UFUNCTION(BlueprintCallable)
	int GetMaxHealth();

	UFUNCTION(BlueprintCallable)
	int GetMovementCost();

	UFUNCTION(BlueprintCallable)
	int GetAttackCost();

	UFUNCTION(BlueprintCallable)
	int GetSpellCost();

	UFUNCTION(BlueprintCallable)
	bool GetIsSelected();

	UFUNCTION(BlueprintCallable)
	bool GetIsHell();

	// Setter for units stats
	UFUNCTION(BlueprintCallable)
	void SetAttack(int a);

	UFUNCTION(BlueprintCallable)
	void SetDefense(int d);

	UFUNCTION(BlueprintCallable)
	void SetCurrentHealth(int ch);

	UFUNCTION(BlueprintCallable)
	void SetMaxHealth(int mh);

	UFUNCTION(BlueprintCallable)
	void SetMovementCost(int mc);

	UFUNCTION(BlueprintCallable)
	void SetAttackCost(int ac);

	UFUNCTION(BlueprintCallable)
	void SetSpellCost(int sc);

	UFUNCTION(BlueprintCallable)
	void SetIsSelected(bool s);

	UFUNCTION(BlueprintCallable)
	void SetIsHell(bool h);

};
