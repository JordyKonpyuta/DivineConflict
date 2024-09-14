// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "F_DC_TileData.h"
#include "GameFramework/Actor.h"
#include "InteractInterface.h"
#include "Grid.generated.h"



class Ainstancedstaticmesh;
class UGridPath;
class UGridInfo;


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

	
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	UPROPERTY(EditAnywhere, Category = "GridElement", meta = (AllowPrivate))
	TMap <FIntVector2, FDC_TileData> GridData;

	UPROPERTY( EditAnywhere, Category = "GridElement")
	FDC_TileData test = FDC_TileData(FIntVector2(0, 0), E_DC_TileTypp::Normal, FTransform3d(FVector(0, 0, 0)), TArray<E_DC_TileState>(), nullptr, nullptr);

	
	FVector3d TraceHitGround(FVector Location);

	UFUNCTION(Category = "GridElement", BlueprintCallable,CallInEditor)
	void SpawnGrid();

	UFUNCTION( Category = "GridElement", BlueprintCallable,CallInEditor)
	void TestPathfinding();

	FVector SnapVectorToVector(FVector InVector, const FVector InSnapTo);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Blueprintable, EditAnywhere, Category = "GridElement")
	FIntVector2 GridSize = FIntVector2(10, 10);

	UPROPERTY(Blueprintable, EditAnywhere, Category = "GridElement")
	FVector TileSize = FVector(100, 100, 100);

	UFUNCTION( Category = "GridElement")
	bool IsTileWalkable(FIntVector2 Index);

	bool IsTileTypeWalkable(E_DC_TileTypp Type);

	TMap <FIntVector2, FDC_TileData> GetGridData();

	
	void SetGridData(TMap <FIntVector2, FDC_TileData> Data);

	FIntVector2 ConvertLocationToIndex(FVector Location);

	FVector3d ConvertIndexToLocation(FIntVector2 Index);
	

};
