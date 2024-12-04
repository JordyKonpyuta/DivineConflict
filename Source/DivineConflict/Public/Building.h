// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomPlayerState.h"
#include "EnumsList.h"
#include "InteractInterface.h"
#include "Unit.h"
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
	// --       Components       --
	// ----------------------------

	UPROPERTY(Blueprintable, VisibleAnywhere, BlueprintReadWrite, Category = "Building")
	TObjectPtr<USkeletalMeshComponent> StaticMeshBuilding = nullptr;

	UPROPERTY(Blueprintable, EditAnywhere, BlueprintReadWrite, Category = "Building")
	TObjectPtr<UStaticMeshComponent> StaticMeshPlane = nullptr;

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Building")
	TObjectPtr<USceneComponent> SceneComp = nullptr;
	
	// ----------------------------
	// --        Materials       --
	// ----------------------------

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInterface>> AllMaterials;
	
	// ----------------------------
	// --       References       --
	// ----------------------------

	// Player
	UPROPERTY(Blueprintable)
	TObjectPtr<ACustomPlayerController> PlayerControllerRef = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TObjectPtr<ACustomPlayerState> OwnerPlayerState = nullptr;

	UPROPERTY()
	TObjectPtr<ACustomGameState> GameStateRef = nullptr;
	
	// Units
	UPROPERTY(Blueprintable, BlueprintReadOnly)
	TObjectPtr<UBuildingSpawnLocation> BuildingSpawnLocationRef = nullptr;

	UPROPERTY(VisibleAnywhere , BlueprintReadOnly, Category = "UnitSelection")
	TObjectPtr<AUnit> UnitRef = nullptr;

	// Grid
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	TObjectPtr<AGrid> Grid = nullptr;
	
	// Spawn
	UPROPERTY()
	TObjectPtr<AUnit> UnitSpawned = nullptr;
	
	// ----------------------------
	// --       Enumerators      --
	// ----------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player")
	EPlayer PlayerOwner = EPlayer::P_Neutral;

	UPROPERTY(VisibleAnywhere , BlueprintReadOnly, Category = "UnitType")
	EUnitType UnitProduced;
	
	// ----------------------------
	// --     Tile Management    --
	// ----------------------------
	
	UPROPERTY(Blueprintable, BlueprintReadOnly, Category="Grid")
	TArray<FIntPoint>AllSpawnLoc;

	UPROPERTY(Blueprintable, BlueprintReadOnly, Category="Grid")
	TArray<FIntPoint> SpawnLocRef;

	UPROPERTY(Blueprintable, BlueprintReadOnly, Category="Grid")
	TArray<FIntPoint> MovementOptions;

	UPROPERTY(Blueprintable, BlueprintReadOnly, Category="Grid")
	bool MovementOptionsReady = false;

	// ----------------------------
	// --        Garrison        --
	// ----------------------------

	UPROPERTY(VisibleAnywhere , BlueprintReadOnly, Replicated, Category = "UnitSelection")
	bool GarrisonFull = false;
	
	UPROPERTY(Blueprintable, BlueprintReadOnly, Category="Grid")
	FVector UnitPosition = FVector(0,0,0);

	// ----------------------------
	// --         Spawn          --
	// ----------------------------

	UPROPERTY()
	bool bHasSpawned = false;
	
protected:
	// ----------------------------
	// --      Unit (Type)       --
	// ----------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	EBuildingList BuildingList;

	// ----------------------------
	// --     Unit (Position)    --
	// ----------------------------
	
	UPROPERTY()
	FIntPoint GridPosition = FIntPoint(0, 0);

	// UFUNCTIONS
public:	
	// ----------------------------
	// --     Initialisation     --
	// ----------------------------

	ABuilding();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_InitStartBuilding(AUnit* UnitSp);
	
	// ----------------------------
	// --        Overrides       --
	// ----------------------------
	
	virtual void Tick(float DeltaTime) override;
	
	virtual bool Interact_Implementation(ACustomPlayerController* PlayerController) override;
	
	// ----------------------------
	// --  Actions (Preparation) --
	// ----------------------------

	UFUNCTION()
	void BuildingPreAction(AUnit* UnitSp);

	// ----------------------------
	// --         Actions        --
	// ----------------------------

	UFUNCTION(BlueprintCallable, Category = "Player")
	void BuildingAction();
	
	// ----------------------------
	// -  Movements (Preparation) -
	// ----------------------------

	UFUNCTION()
	void PrepareMovements();
	
	// ---------------------------- //
	// --         GETTERS        -- //
	// ---------------------------- //

	// Grid Position
	UFUNCTION()
	FIntPoint GetGridPosition();

	// Type
	UFUNCTION()
	EBuildingList GetBuildingList();

	// ---------------------------- //
	// --         SETTERS        -- //
	// ---------------------------- //

	// Grid Position
	void SetGridPosition(FIntPoint GridPosition);

	// Owner & Type
	UFUNCTION(BlueprintCallable, Category = "Player")
	void SwitchOwner(ACustomPlayerState* NewOwner);

	UFUNCTION(Server, Reliable)
	void Server_SwitchOwner(ACustomPlayerState* NewOwner);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SwitchOwner(ACustomPlayerState* NewOwner);
	
protected:
	// ----------------------------
	// --        Override        --
	// ----------------------------
	virtual void BeginPlay() override;
	
	// ----------------------------
	// --          Turns         --
	// ----------------------------
	
	UFUNCTION(Blueprintable, Category = "Player")
	bool IsPlayerPassive(ACustomPlayerController* PlayerController);

	UFUNCTION()
	void OnTurnChanged();
};