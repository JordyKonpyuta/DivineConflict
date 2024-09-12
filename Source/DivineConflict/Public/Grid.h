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

	virtual bool Interact_Implementation(ACustomPlayerController* PlayerController) override;
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	UPROPERTY(EditAnywhere, Category = "GridElement", meta = (AllowPrivate))
	TMap <FVector2D, FDC_TileData> GridData;
	
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
	FVector2D GridSize = FVector2D(10, 10);

	UPROPERTY(Blueprintable, EditAnywhere, Category = "GridElement")
	FVector TileSize = FVector(100, 100, 100);

	UFUNCTION( Category = "GridElement", BlueprintCallable)
	bool IsTileWalkable(FVector2D Index);

	bool IsTileTypeWalkable(E_DC_TileTypp Type);

	TMap <FVector2D, FDC_TileData> GetGridData();

	FVector2d ConvertLocationToIndex(FVector Location);

	FVector3d ConvertIndexToLocation(FVector2d Index);
	

};
