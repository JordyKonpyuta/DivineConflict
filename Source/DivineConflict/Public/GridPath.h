// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomPlayerController.h"
#include "Components/ActorComponent.h"
#include "F_DC_TileData.h"
#include "GridPath.generated.h"

class AGrid;


//structure for the pathdata
USTRUCT(BlueprintType)
struct FPathData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "PathData")
	FIntPoint Index = FIntPoint(-999, -999);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PathData")
	int costEntrerToTile = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PathData")
	int MinCostToTarget = 9999999;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PathData")
	int CostFormStart = 9999999;
	UPROPERTY(EditAnywhere,  Category = "PathData")
	FIntPoint PreviewsIndex = FIntPoint(-999, -999);
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DIVINECONFLICT_API UGridPath : public UActorComponent
{
	GENERATED_BODY()

		// UPROPERTIES
public:	

	// ----------------------------
	// References
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TObjectPtr<AGrid> Grid = nullptr;

protected:

	// ----------------------------
	// Pre-Movement Info
	
	UPROPERTY( VisibleAnywhere, Category = "Grid", meta = (AllowPrivate = "true"))
	FIntPoint StartPoint = FIntPoint(-999	,-999);
	
	UPROPERTY( VisibleAnywhere,  Category = "Grid", meta = (AllowPrivate = "true"))
	FIntPoint EndPoint = FIntPoint(-999, -999);
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	int MaxLenght = 0;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	bool bIsReachable = false;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	bool bIsEscalation = false;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	bool bIsAttackable = false;
	
	// ----------------------------
	// Movement Output
	
	UPROPERTY( VisibleAnywhere, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<FIntPoint> Path;
	
	UPROPERTY( VisibleAnywhere,  Category = "Grid", meta = (AllowPrivate = "true"))
	TMap<FIntPoint, FPathData> PathData;
	
	// ----------------------------
	// Tiles Check
	
	UPROPERTY( VisibleAnywhere,  Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<FIntPoint> DiscoverTileIndex;
	
	UPROPERTY(	VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<int> DiscoverTileSortingCost;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	FPathData CurrentDiscoverTile;
	
	UPROPERTY(	VisibleAnywhere,  Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<FIntPoint> AnalyseTileIndex;
	
	// ----------------------------
	// Neighbours
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<FPathData> CurrentsNeighbors;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	FPathData CurrentNeighbors;
	
	// UFUNCTIONS
public:	
	// ----------------------------
	// Constructor
	
	UGridPath();
	
	// ----------------------------
	// Overrides
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
	// ----------------------------
	// Height Check
	
	bool IsValidHeigh(FDC_TileData* IndextestData, FDC_TileData* CurrentIndexData);
	
	bool IsValidHeighWarrior(FDC_TileData* IndextestData, FDC_TileData* CurrentIndexData);

	// ----------------------------
	// Find Neighbours

	TArray<FIntPoint> FindTileNeighbors(FIntPoint Index);
	
	// ----------------------------
	// Generate Pathfinding

	//Pathfinding and Path Reachable
	UFUNCTION()
	TArray<FIntPoint> FindPath(FIntPoint Start, FIntPoint End , bool IsReachable, int PathLenght, bool IsEscalation, bool attackable);

	// ----------------------------
	//Path Reachable
	TArray<FIntPoint> NewFindPath(FIntPoint Start, ACustomPlayerController* CustomPlayerController);

	//-----------------------------
	// Path Reachable Sp Mage

	TArray<FIntPoint> NewFindPathSpMage(FIntPoint Start, ACustomPlayerController* CustomPlayerController);
	
	// ----------------------------
	// SETTER
	void SetGrid(AGrid* GridRef);

protected:
	// ----------------------------
	// Override
	virtual void BeginPlay() override;
	
	// ----------------------------
	// Path
	
	TArray<FIntPoint> GeneratePath();
	
	void ClearGeneratedPath();
	
	// ----------------------------
	// Tile Discovery (set-up)
	
	void DiscoverTile(FPathData TilePath);
	
	int MinimulCostBetweenTwoTile(FIntPoint Index1, FIntPoint Index2);
	
	void InserTileDiscoverList(FPathData TilePath);
	
	bool IsInputDataValid(FIntPoint Start, FIntPoint End);

	// ----------------------------
	// Tile Discovery (Process)
	
	FPathData PullCheapestTileOutOfDiscoverList();
	
	bool AnalyseNextDiscoverTile();
	
	// ----------------------------
	// Neighbours
	
	bool DiscoverNextNeighbors();
	// ----------------------------
	// Test viladity of Tile for walkable
	TArray<FPathData> GetValidTileNeighbors(FIntPoint Index);
};

