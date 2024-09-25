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

public:	
	// Sets default values for this component's properties
	UBuildingSpawnLocation();

	UPROPERTY(NotBlueprintable)
	ABuilding* BuildingRef;

	UPROPERTY(Blueprintable, BlueprintReadOnly, Category="Refs")
	AGrid* GridRef;
	
	UPROPERTY(Blueprintable, EditAnywhere, Category="Grid")
	TArray<bool>ArraySpawnableIndex;

	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(NotBlueprintable)
	int GridInstance;
	
	UPROPERTY(NotBlueprintable)
	FIntPoint CurrentTileIndex;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(Blueprintable)
	void SpawnGridColors(TArray<FIntPoint> AllSpawnLocationIndex);
	
	UFUNCTION(Blueprintable)
	void DeSpawnGridColors(TArray<FIntPoint> AllSpawnLocationIndex);
};
