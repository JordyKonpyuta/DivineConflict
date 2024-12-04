// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GridInfo.generated.h"

class AUpwall;
class ATower;
class AGrid;
class AUnit;
class ABuilding;
class ABase;
class UFDC_GridData;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DIVINECONFLICT_API UGridInfo : public UActorComponent
{
	GENERATED_BODY()

	
	// UPROPERTIES
public:	

protected:
	// ----------------------------
	// --        References      --
	// ----------------------------
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TObjectPtr<AGrid> Grid = nullptr;
	
	// ----------------------------
	// --   All Objects in Grid  --
	// ----------------------------
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<TObjectPtr<AUnit>> UnitsCombat;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<TObjectPtr<ABuilding>> BuildingGrid;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<TObjectPtr<ABase>> BasesGrid;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<TObjectPtr<ATower>> TowerGrid;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<TObjectPtr<AUpwall>> ClimbableGrid;

	
	// UFUNCTIONS
public:	
	// ----------------------------
	// --       Constructor      --
	// ----------------------------

	UGridInfo();
	
	// ----------------------------
	// --        Overrides       --
	// ----------------------------
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// ----------------------------
	// --      Unit On Grid      --
	// ----------------------------
	
	UFUNCTION( Category = "GridElement",CallInEditor)
	void AddUnitInGrid(FIntPoint GridPosition, AUnit* Unit);

	UFUNCTION(NetMulticast,Reliable, Category = "GridElement",CallInEditor)
	void Multi_SetUnitIndexOnGrid(const FIntPoint GridPosition, const AUnit* Unit);

	UFUNCTION(Server,Reliable, Category = "GridElement")
	void Server_SetUnitIndexOnGrid(const FIntPoint GridPosition, const AUnit* Unit);

	UFUNCTION( Category = "GridElement",CallInEditor)
	void RemoveUnitInGrid(AUnit* Unit);
	
	// ----------------------------
	// --    Building On Grid    --
	// ----------------------------
	
	UFUNCTION(	Category = "GridElement",CallInEditor)
	void AddBuildingOnGrid(FIntPoint GridPosition, ABuilding* Building);

	UFUNCTION( Category = "GridElement",CallInEditor)
	void SetBuildingOnGrid(FIntPoint GridPosition, ABuilding* Building);

	// ----------------------------
	// --      Base On Grid      --
	// ----------------------------
	
	UFUNCTION( Category = "GridElement",CallInEditor)
	void AddBaseOnGrid(FIntPoint GridPosition, ABase* Base);

	UFUNCTION(	Category = "GridElement",CallInEditor)
	void SetBaseOnGrid(FIntPoint GridPosition, ABase* Base);

	UFUNCTION( NetMulticast,Reliable, Category = "GridElement",CallInEditor)
	void Multi_SetBaseOnGrid(const FIntPoint GridPosition,const ABase* Base);
	
	// ----------------------------
	// --      Tower On Grid     --
	// ----------------------------
	
	UFUNCTION( Category = "GridElement",CallInEditor)
	void AddTowerOnGrid(FIntPoint GridPosition, ATower* Tower);

	UFUNCTION(	Category = "GridElement",CallInEditor)
	void SetTowerOnGrid(FIntPoint GridPosition, ATower* Tower);
	
	// ----------------------------
	// -- Climbable Area On Grid --
	// ----------------------------
	
	UFUNCTION( Category = "GridElement",CallInEditor)
	void AddClimbableOnGrid(FIntPoint GridPosition, AUpwall* Upwall);

	UFUNCTION(	Category = "GridElement",CallInEditor)
	void SetClimbableOnGrid(FIntPoint GridPosition, AUpwall* Upwall);
	
	// ---------------------------- //
	// --         SETTERS        -- //
	// ---------------------------- //

	UFUNCTION()
	void SetGrid(AGrid* GridRef);

protected:
	// ----------------------------
	// --        Overrides       --
	// ----------------------------
	
	virtual void BeginPlay() override;
	
};
