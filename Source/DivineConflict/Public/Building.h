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

UCLASS()

class DIVINECONFLICT_API ABuilding : public AActor, public IInteractInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ABuilding();

	virtual bool Interact_Implementation(ACustomPlayerController* PlayerController) override;
	
	UPROPERTY(Blueprintable, BlueprintReadOnly)
	UBuildingSpawnLocation* BuildingSpawnLocationRef;
	
	UPROPERTY(Blueprintable)
	ACustomPlayerController* PlayerControllerRef;

	UPROPERTY(Blueprintable, EditAnywhere, BlueprintReadWrite, Category = "Building")
	UStaticMeshComponent* StaticMeshBuilding;

	UPROPERTY(Blueprintable, EditAnywhere, BlueprintReadWrite, Category = "Building")
	UStaticMeshComponent* StaticMeshPlane;

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Building")
	USceneComponent* SceneComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player")
	EPlayer PlayerOwner = EPlayer::P_Neutral;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	ACustomPlayerState* OwnerPlayerState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	AGrid* Grid;

	UPROPERTY(Blueprintable, BlueprintReadOnly, Category="Grid")
	TArray<FIntPoint>AllSpawnLoc;

	UPROPERTY(Blueprintable, BlueprintReadOnly, Category="Grid")
	TArray<FIntPoint> SpawnLocRef;

	UPROPERTY(VisibleAnywhere , BlueprintReadOnly, Category = "UnitType")
	EUnitType UnitProduced;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	EBuildingList BuildingList;

	UPROPERTY()
	FIntPoint GridPosition = FIntPoint(0, 0);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FIntPoint GetGridPosition();

	void SetGridPosition(FIntPoint GridPosition);
	
protected:

	UFUNCTION(BlueprintCallable, Category = "Player")
	void SwitchOwner(ACustomPlayerState* NewOwner);

	UFUNCTION(Blueprintable, Category = "Player")
	bool IsPlayerPassive(ACustomPlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Player")
	void SpawnUnitFromBuilding(FIntPoint SpawnLocation);
	
};
