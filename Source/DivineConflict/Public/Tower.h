// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomPlayerController.h"
#include "EnumsList.h"
#include "GameFramework/Actor.h"
#include "Tower.generated.h"

class AUnit;
class AGrid;
class AProjectile;

UCLASS()
class DIVINECONFLICT_API ATower : public AActor
{
	GENERATED_BODY()

	// UPROPERTIES
public:	
	// ----------------------------
	// --       Components       --
	// ----------------------------
	
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;
	
	// ----------------------------
	// --       References       --
	// ----------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	TObjectPtr<AGrid> Grid = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controller")
	TObjectPtr<ACustomPlayerController> PlayerController = nullptr;
	
	UPROPERTY(Replicated)
	TObjectPtr<AUnit> UnitToAttack = nullptr;

	// ----------------------------
	// Can Attack
	
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Bool")
	bool bCanAttack = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bool")
	bool bIsSelected = false;
	
	// ----------------------------
	// Attack
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	TArray<FIntPoint> TilesInRange = {FIntPoint(-999,-999)};

	UPROPERTY(Replicated)
	FIntPoint UnitToAttackPosition = FIntPoint(-999, -999);
	
	UPROPERTY()
	TObjectPtr<AProjectile> Projectile;
	
	// ----------------------------
	// Tower Upgrade

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Level = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxLevel = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int WoodUpgradePrice = -1000000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int StoneUpgradePrice = -1000000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int GoldUpgradePrice = -1000000;

protected:

	// ----------------------------
	// Stat
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Stats")
	int Attack = 0;
	
	// ----------------------------
	// Owner
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Enum")
	EPlayer PlayerOwner = EPlayer::P_Neutral;
	
	// ----------------------------
	// Position
	
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Grid")
	FIntPoint GridPosition = FIntPoint(-999, -999);

	// UFUNCTIONS
public:	
	// ----------------------------
	// Constructor
	
	ATower();
	
	// ----------------------------
	// Overrides
	
	virtual void Tick(float DeltaTime) override;
	
	// ----------------------------
	// Components

	UFUNCTION(BlueprintNativeEvent)
	void SetMesh();
	
	// ----------------------------
	// Attack

	UFUNCTION()
	void PrepareAttack(AUnit* UnitAttack);

	UFUNCTION(BlueprintCallable)
	void AttackUnit(AUnit* UnitAttacking, ACustomPlayerController* PlayerControllerAttacking);
	
	// ----------------------------
	// Tower Upgrade

	UFUNCTION(BlueprintCallable)
	void UpgradeTower();
	
	// ----------------------------
	// GETTERS
	
	UFUNCTION(BlueprintCallable)
	int GetAttack();
	
	UFUNCTION(BlueprintCallable)
	EPlayer GetPlayerOwner();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetCanAttack();
	
	FIntPoint GetGridPosition();
	
	// ----------------------------
	// SETTERS
	
	UFUNCTION(BlueprintCallable)
	void SetAttack(int NewAttack);
	
	UFUNCTION(BlueprintCallable)
	void SetPlayerOwner(EPlayer NewPlayerOwner);
	
	UFUNCTION(BlueprintCallable)
	void SetCanAttack(bool NewCanAttack);

	UFUNCTION()
	void SetGridPosition(FIntPoint NewGridPosition);

protected:
	// ----------------------------
	// Overrides

	virtual void BeginPlay() override;
};
