// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GridInfo.generated.h"

class AGrid;
class AUnit;
class ASpawner;

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
	void addSpawnUnitOnGrid(FIntPoint GridPosition, ASpawner* Spawner);

	UFUNCTION( Category = "GridElement",CallInEditor)
	void SetSpawnUnitOnGrid(FIntPoint GridPosition, ASpawner* Spawner);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	AGrid* Grid;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<AUnit*> UnitsCombat;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<ASpawner*> SpawnersGrid;

	

	
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//sets the grid reference
	void SetGrid(AGrid* GridRef);
		
};
