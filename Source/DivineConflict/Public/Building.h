// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomPlayerState.h"
#include "EnumsList.h"
#include "GameFramework/Actor.h"
#include "Building.generated.h"

class AGrid;
class ACustomPlayerController;

UCLASS()

class DIVINECONFLICT_API ABuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuilding();

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Building")
	UStaticMeshComponent* StaticMeshBuilding;

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category="Player")
	EPlayer PlayerOwner;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	ACustomPlayerState* OwnerPlayerState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	AGrid* Grid;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	EBuildingList BuildingList;

	UPROPERTY(VisibleAnywhere , BlueprintReadOnly, Category = "UnitType")
	EUnitType UnitProduced;

	UPROPERTY()
	FIntPoint GridPosition = FIntPoint(0, 0);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FIntPoint GetGridPosition();

	void SetGridPosition(FIntPoint GridPosition);
	
protected:
	UFUNCTION(BlueprintCallable, Category = "UnitType")
	void SpawnUnit();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void SwitchOwner(ACustomPlayerState* NewOwner);

	UFUNCTION(Blueprintable, Category = "Player")
	bool IsPlayerPassive(ACustomPlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Player")
	void SpawnUnitFromBuilding(FIntPoint SpawnLocation);
	
};
