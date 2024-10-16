// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuildingSpawnLocation.generated.h"

class AGrid;
class ABuilding;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DIVINECONFLICT_API UBuildingSpawnLocation : public UActorComponent
{
	GENERATED_BODY()

	//UPROPERTIES
public:	
	// ----------------------------
	// References
	UPROPERTY(NotBlueprintable)
	ABuilding* BuildingRef;

	UPROPERTY(Blueprintable, BlueprintReadOnly, Category="Refs")
	AGrid* GridRef;
	
	// ----------------------------
	// Spawn
	UPROPERTY(Blueprintable, EditAnywhere, Category="Grid")
	TArray<bool>ArraySpawnableIndex;

	
protected:
	// ----------------------------
	// Grid Reader
	
	UPROPERTY(NotBlueprintable)
	int GridInstance;

	// UFUNCTIONS
public:	
	// ----------------------------
	// Initialisation
	
	UBuildingSpawnLocation();
	
	// ----------------------------
	// Overrides
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// ----------------------------
	// Grid Colors
	
	UFUNCTION(Blueprintable, BlueprintCallable)
	void SpawnGridColors(TArray<FIntPoint> AllSpawnLocationIndex);
	
	UFUNCTION(Blueprintable, BlueprintCallable)
	void DeSpawnGridColors(TArray<FIntPoint> AllSpawnLocationIndex);

private:
	
	// ----------------------------
	// Overrides
	virtual void BeginPlay() override;
};
