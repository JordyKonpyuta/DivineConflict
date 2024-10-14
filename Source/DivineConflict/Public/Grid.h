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

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = "true"))
    UInstancedStaticMeshComponent* GridMesh;	


public:	
	// Sets default values for this actor's properties
	AGrid();

	
	
	UPROPERTY(EditAnywhere , Category = "GridElement")
	UGridPath* GridPath = nullptr;

	UPROPERTY(EditAnywhere, Category = "GridElement")
	UGridInfo* GridInfo = nullptr;

	UPROPERTY(EditAnywhere, Category = "GridElement")
	UGridVisual * GridVisual = nullptr;
	
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	

	UPROPERTY( EditAnywhere, Category = "GridElement")
	TArray<FIntPoint> InstanceArray;

	
	FHitResult TraceHitGround(FVector Location);

	UFUNCTION(Category = "GridElement", BlueprintCallable,CallInEditor)
	void SpawnGrid();

	
	UFUNCTION( Category = "GridElement", BlueprintCallable,CallInEditor)
	void TestPathfinding();

	UFUNCTION(CallInEditor, BlueprintCallable,Category = "GridElement")
	void TestReachedPath();

	UFUNCTION(CallInEditor, BlueprintCallable,Category = "GridElement")
	void TestReachwithCliming();
	

	FVector SnapVectorToVector(FVector InVector, const FVector InSnapTo);

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "GridElement", meta = (AllowPrivate))
	TMap <FIntPoint, FDC_TileData> GridData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GridElement")
	FIntPoint GridSize = FIntPoint(10, 10);

	UPROPERTY(Blueprintable, EditAnywhere, Category = "GridElement")
	FVector TileSize = FVector(100, 100, 100);
	
	
	UFUNCTION( Category = "GridElement")
	bool IsTileWalkable(FIntPoint Index);

	bool IsTileTypeWalkable(EDC_TileType Type);

	TMap <FIntPoint, FDC_TileData>* GetGridData();
	
	void SetGridData(TMap <FIntPoint, FDC_TileData> Data);

	FIntPoint ConvertLocationToIndex(FVector3d Location);

	FVector3d ConvertIndexToLocation(FIntPoint Index);

	void RemoveInstance(FIntPoint Index);

	int AddInstance(FIntPoint Index, FTransform3d Transform);

	void UpdateColor(int I, FLinearColor InColor, float	Alpha);


};
