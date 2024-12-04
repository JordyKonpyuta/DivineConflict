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
	// --       References       --
	// ----------------------------
	UPROPERTY(NotBlueprintable)
	TObjectPtr<ABuilding> BuildingRef = nullptr;

	UPROPERTY(Blueprintable, BlueprintReadOnly, Category="Refs")
	TObjectPtr<AGrid> GridRef = nullptr;

	
protected:
	// ----------------------------
	// --       Grid Reader      --
	// ----------------------------
	
	UPROPERTY(NotBlueprintable)
	int GridInstance;

	// UFUNCTIONS
public:	
	// ----------------------------
	// --     Initialisation     --
	// ----------------------------
	
	UBuildingSpawnLocation();
	
	// ----------------------------
	// --        Override        --
	// ----------------------------
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// ----------------------------
	// --       Grid Colors      --
	// ----------------------------
	
	UFUNCTION(Blueprintable, BlueprintCallable)
	void SpawnGridColors(TArray<FIntPoint> AllSpawnLocationIndex);
	
	UFUNCTION(Blueprintable, BlueprintCallable)
	void DespawnGridColors(TArray<FIntPoint> AllSpawnLocationIndex);

private:
	// ----------------------------
	// --        Override        --
	// ----------------------------
	virtual void BeginPlay() override;
};
