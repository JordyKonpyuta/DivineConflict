// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GridInfo.generated.h"

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

	

public:	
	// Sets default values for this component's properties
	UGridInfo();

	UFUNCTION( Category = "GridElement",CallInEditor)
	void AddUnitInGrid(FIntPoint GridPosition, AUnit* Unit);

	UFUNCTION( Category = "GridElement",CallInEditor)
	void setUnitIndexOnGrid(FIntPoint GridPosition, AUnit *Unit);

	UFUNCTION( Category = "GridElement",CallInEditor)
	void RemoveUnitInGrid(AUnit* Unit);

	UFUNCTION(	Category = "GridElement",CallInEditor)
	void addBuildingOnGrid(FIntPoint GridPosition, ABuilding* Building);

	UFUNCTION( Category = "GridElement",CallInEditor)
	void SetBuildingOnGrid(FIntPoint GridPosition, ABuilding* Building);

	UFUNCTION( Category = "GridElement",CallInEditor)
	void addBaseOnGrid(FIntPoint GridPosition, ABase* Base);

	UFUNCTION(	Category = "GridElement",CallInEditor)
	void SetBaseOnGrid(FIntPoint GridPosition, ABase* Base);

	UFUNCTION( Category = "GridElement",CallInEditor)
	void addTowerOnGrid(FIntPoint GridPosition, ATower* Tower);

	UFUNCTION(	Category = "GridElement",CallInEditor)
	void SetTowerOnGrid(FIntPoint GridPosition, ATower* Tower);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	AGrid* Grid;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<AUnit*> UnitsCombat;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<ABuilding*> BuildingGrid;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<ABase*> BasesGrid;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<ATower*> TowerGrid;

	
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//sets the grid reference
	void SetGrid(AGrid* GridRef);
		
};
