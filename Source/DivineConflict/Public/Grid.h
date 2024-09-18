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
	TMap <FIntPoint, FDC_TileData> GridData;

	UPROPERTY( EditAnywhere, Category = "GridElement")
	FDC_TileData test = FDC_TileData(FIntPoint(0, 0), E_DC_TileTypp::Normal, FTransform3d(FVector(0, 0, 0)), TArray<E_DC_TileState>(), nullptr, nullptr);

	
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
	FIntPoint GridSize = FIntPoint(10, 10);

	UPROPERTY(Blueprintable, EditAnywhere, Category = "GridElement")
	FVector TileSize = FVector(100, 100, 100);

	UFUNCTION( Category = "GridElement")
	bool IsTileWalkable(FIntPoint Index);

	bool IsTileTypeWalkable(E_DC_TileTypp Type);

	TMap <FIntPoint, FDC_TileData> GetGridData();

	
	void SetGridData(TMap <FIntPoint, FDC_TileData> Data);

	FIntPoint ConvertLocationToIndex(FVector Location);

	FVector3d ConvertIndexToLocation(FIntPoint Index);
	

};
