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
	// --       Components       --
	// ----------------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInstancedStaticMeshComponent> GridMesh = nullptr;	
	
	// ----------------------------
	// --       References       --
	// ----------------------------
	
	UPROPERTY(EditAnywhere , Category = "GridElement")
	TObjectPtr<UGridPath> GridPath = nullptr;

	UPROPERTY(EditAnywhere, Category = "GridElement")
	TObjectPtr<UGridInfo> GridInfo = nullptr;

	UPROPERTY(EditAnywhere, Category = "GridElement")
	TObjectPtr<UGridVisual> GridVisual = nullptr;

	// ----------------------------
	// --     Grid Properties    --
	// ----------------------------
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "GridElement", meta = (AllowPrivate))
	TMap <FIntPoint, FDC_TileData> GridData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GridElement")
	FIntPoint GridSize = FIntPoint(10, 10);

	UPROPERTY(Blueprintable, EditAnywhere, Category = "GridElement")
	FVector TileSize = FVector(100, 100, 100);
	
protected:
	// ----------------------------
	// --        Instances       --
	// ----------------------------
	
	UPROPERTY( EditAnywhere, Category = "GridElement")
	TArray<FIntPoint> InstanceArray;
	
	// UFUNCTIONS
public:	
	// ----------------------------
	// --       Constructor      --
	// ----------------------------
	
	AGrid();
	
	// ----------------------------
	// --        Overrides       --
	// ----------------------------

	UFUNCTION()
	virtual void Tick(float DeltaTime) override;
	
	// ----------------------------
	// --   Check Grid Elements  --
	// ----------------------------
	
	UFUNCTION( Category = "GridElement")
	bool IsTileWalkable(FIntPoint Index , bool Attacking);

	UFUNCTION()
	bool IsTileTypeWalkable(EDC_TileType Type);

	UFUNCTION()
	bool CanAttackTileType(EDC_TileType Type);
	
	// ----------------------------
	// --       Conversions      --
	// ----------------------------

	UFUNCTION()
	FIntPoint ConvertLocationToIndex(FVector3d Location);

	UFUNCTION()
	FVector3d ConvertIndexToLocation(FIntPoint Index);

	// ----------------------------
	// --    Instances Editor    --
	// ----------------------------

	UFUNCTION()
	int AddInstance(FIntPoint Index, FTransform3d Transform);

	UFUNCTION()
	void RemoveInstance(FIntPoint Index);
	
	// ----------------------------
	// --          Color         --
	// ----------------------------

	UFUNCTION()
	void UpdateColor(int I, FLinearColor InColor, float	Alpha);

	// ----------------------------
	// --        Grid Data       --
	// ----------------------------
	
	TMap <FIntPoint, FDC_TileData>* GetGridData();
	
	// ----------------------------
	// --         SETTERS        --
	// ----------------------------

	UFUNCTION()
	void SetGridData(TMap <FIntPoint, FDC_TileData> Data);

protected:
	// ----------------------------
	// --        Overrides       --
	// ----------------------------
	
	virtual void BeginPlay() override;
	
	// ----------------------------
	// --       Spawn Grid       --
	// ----------------------------

	UFUNCTION(Category = "GridElement", BlueprintCallable,CallInEditor)
	void SpawnGrid();

	UFUNCTION()
	FHitResult TraceHitGround(FVector Location);

	UFUNCTION()
	FVector SnapVectorToVector(FVector InVector, const FVector InSnapTo);

};
