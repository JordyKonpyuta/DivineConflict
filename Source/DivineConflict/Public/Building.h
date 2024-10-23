// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomPlayerState.h"
#include "EnumsList.h"
#include "InteractInterface.h"
#include "GameFramework/Actor.h"
#include "Building.generated.h"

class AGrid;
class ACustomPlayerController;
class UBuildingSpawnLocation;
class AUnit;

UCLASS()

class DIVINECONFLICT_API ABuilding : public AActor, public IInteractInterface
{
	GENERATED_BODY()

	// UPROPERTIES
public:
	// ----------------------------
	// Components

	UPROPERTY(Blueprintable, VisibleAnywhere, BlueprintReadWrite, Category = "Building")
	USkeletalMeshComponent* StaticMeshBuilding;

	UPROPERTY(Blueprintable, EditAnywhere, BlueprintReadWrite, Category = "Building")
	UStaticMeshComponent* StaticMeshPlane;

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Building")
	USceneComponent* SceneComp;
	
	// ----------------------------
	// Materials

	UPROPERTY()
	TArray<UMaterialInterface*> AllMaterials;
	
	// ----------------------------
	// References

	// Player
	UPROPERTY(Blueprintable)
	ACustomPlayerController* PlayerControllerRef;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	ACustomPlayerState* OwnerPlayerState;

	UPROPERTY()
	ACustomGameState* GameStateRef;
	
	// Units
	UPROPERTY(Blueprintable, BlueprintReadOnly)
	UBuildingSpawnLocation* BuildingSpawnLocationRef;

	UPROPERTY(VisibleAnywhere , BlueprintReadOnly, Category = "UnitSelection")
	AUnit* UnitRef = nullptr;

	// Grid
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	AGrid* Grid;
	
	// Spawn
	UPROPERTY()
	AUnit* UnitSpawned = nullptr;
	
	// ----------------------------
	// Enumerators

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player")
	EPlayer PlayerOwner = EPlayer::P_Neutral;

	UPROPERTY(VisibleAnywhere , BlueprintReadOnly, Category = "UnitType")
	EUnitType UnitProduced;
	
	// ----------------------------
	// Tile Management
	
	UPROPERTY(Blueprintable, BlueprintReadOnly, Category="Grid")
	TArray<FIntPoint>AllSpawnLoc;

	UPROPERTY(Blueprintable, BlueprintReadOnly, Category="Grid")
	TArray<FIntPoint> SpawnLocRef;

	// ----------------------------
	// Garrison

	UPROPERTY(VisibleAnywhere , BlueprintReadOnly, Category = "UnitSelection")
	bool GarrisonFull = false;
	
	UPROPERTY(Blueprintable, BlueprintReadOnly, Category="Grid")
	FVector UnitPosition;

	// ----------------------------
	// Spawn

	UPROPERTY()
	bool bHasSpawned = false;
	
protected:
	// ----------------------------
	// UnitType

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	EBuildingList BuildingList;

	// ----------------------------
	// Unit Position
	
	UPROPERTY()
	FIntPoint GridPosition = FIntPoint(0, 0);

	// UFUNCTIONS
public:	
	// ----------------------------
	// Initialisation

	ABuilding();
	
	// ----------------------------
	// Overrides
	
	virtual void Tick(float DeltaTime) override;
	
	virtual bool Interact_Implementation(ACustomPlayerController* PlayerController) override;
	
	// ----------------------------
	// Prepare Actions

	UFUNCTION()
	void BuildingPreAction(AUnit* UnitSp);

	// ----------------------------
	// Actions

	UFUNCTION(BlueprintCallable, Category = "Player")
	void BuildingAction();
	

	// ----------------------------
	// GETTERS

	// Grid Position
	UFUNCTION()
	FIntPoint GetGridPosition();

	// Type
	UFUNCTION()
	EBuildingList GetBuildingList();

	// ----------------------------
	// SETTERS

	// Grid Position
	void SetGridPosition(FIntPoint GridPosition);

	// Owner & Type
	UFUNCTION(BlueprintCallable, Category = "Player")
	void SwitchOwner(ACustomPlayerState* NewOwner);
	
protected:
	// ----------------------------
	// Override
	virtual void BeginPlay() override;
	
	// ----------------------------
	// Spawn
	
	// Cost takes three arguments, in order : wood, stone, gold
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Player")
	void SpawnUnitFromBuilding(const FIntPoint &SpawnLocation, const TArray<int> &Cost);
	
	// ----------------------------
	// Garrison
	
	UFUNCTION(BlueprintCallable, Category = "Unit")
	void removeUnitRef();
	
	// ----------------------------
	// Handling Turns
	
	UFUNCTION(Blueprintable, Category = "Player")
	bool IsPlayerPassive(ACustomPlayerController* PlayerController);

	UFUNCTION()
	void OnTurnChanged();
};