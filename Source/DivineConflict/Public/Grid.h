// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "F_DC_TileData.h"
#include "GameFramework/Actor.h"
#include "InteractInterface.h"
#include "Grid.generated.h"


class UFDC_GridData;
class Ainstancedstaticmesh;
class UGridPath;
class UGridInfo;
class UGridVisual;


UCLASS()
class DIVINECONFLICT_API AGrid : public AActor , public IInteractInterface
{
	GENERATED_BODY()

	

	// UPROPERTIES
public:	
	// ----------------------------
	// Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = "true"))
    UInstancedStaticMeshComponent* GridMesh;	
	
	// ----------------------------
	// References
	
	UPROPERTY(EditAnywhere , Category = "GridElement")
	UGridPath* GridPath = nullptr;

	UPROPERTY(EditAnywhere, Category = "GridElement")
	UGridInfo* GridInfo = nullptr;

	UPROPERTY(EditAnywhere, Category = "GridElement")
	UGridVisual * GridVisual = nullptr;

	// ----------------------------
	// Grid Properties
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "GridElement", meta = (AllowPrivate))
	TMap <FIntPoint, FDC_TileData> GridData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GridElement")
	FIntPoint GridSize = FIntPoint(10, 10);

	UPROPERTY(Blueprintable, EditAnywhere, Category = "GridElement")
	FVector TileSize = FVector(100, 100, 100);
	
protected:
	// ----------------------------
	// Instances
	
	UPROPERTY( EditAnywhere, Category = "GridElement")
	TArray<FIntPoint> InstanceArray;
	
	// UFUNCTIONS
public:	
	// ----------------------------
	// Constructor
	
	AGrid();
	
	// ----------------------------
	// Overrides
	virtual void Tick(float DeltaTime) override;
	
	// ----------------------------
	// Check Grid Element
	
	UFUNCTION( Category = "GridElement")
	bool IsTileWalkable(FIntPoint Index);

	bool IsTileTypeWalkable(EDC_TileType Type);

	// ----------------------------
	// Converts
	
	FIntPoint ConvertLocationToIndex(FVector3d Location);

	FVector3d ConvertIndexToLocation(FIntPoint Index);

	
	// ----------------------------
	// Instances Editor

	int AddInstance(FIntPoint Index, FTransform3d Transform);
	
	void RemoveInstance(FIntPoint Index);
	
	// ----------------------------
	// Color

	void UpdateColor(int I, FLinearColor InColor, float	Alpha);

	// ----------------------------
	// GETTERS

	TMap <FIntPoint, FDC_TileData>* GetGridData();
	
	// ----------------------------
	// SETTERS
	
	void SetGridData(TMap <FIntPoint, FDC_TileData> Data);

protected:
	// ----------------------------
	// Overrides
	
	virtual void BeginPlay() override;
	
	// ----------------------------
	// Spawn Grid

	UFUNCTION(Category = "GridElement", BlueprintCallable,CallInEditor)
	void SpawnGrid();
	
	FHitResult TraceHitGround(FVector Location);

	FVector SnapVectorToVector(FVector InVector, const FVector InSnapTo);

	// ----------------------------
	// Tests
	
	UFUNCTION( Category = "GridElement", BlueprintCallable,CallInEditor)
	void TestPathfinding();

	UFUNCTION(CallInEditor, BlueprintCallable,Category = "GridElement")
	void TestReachedPath();

	UFUNCTION(CallInEditor, BlueprintCallable,Category = "GridElement")
	void TestReachwithCliming();

};
