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
	int CostEntryInTile = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PathData")
	int MinCostToTarget = 9999999;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PathData")
	int CostFormStart = 9999999;
	UPROPERTY(EditAnywhere,  Category = "PathData")
	FIntPoint PreviewIndex = FIntPoint(-999, -999);
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DIVINECONFLICT_API UGridPath : public UActorComponent
{
	GENERATED_BODY()

		// UPROPERTIES
public:	
	// ----------------------------
	// --       References       --
	// ----------------------------
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TObjectPtr<AGrid> Grid = nullptr;

protected:
	
	// ----------------------------
	// --    Pre-Movement Info   --
	// ----------------------------
	
	UPROPERTY( VisibleAnywhere, Category = "Grid", meta = (AllowPrivate = "true"))
	FIntPoint StartPoint = FIntPoint(-999	,-999);
	
	UPROPERTY( VisibleAnywhere,  Category = "Grid", meta = (AllowPrivate = "true"))
	FIntPoint EndPoint = FIntPoint(-999, -999);
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	int MaxLenght = 0;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	bool bIsReachable = false;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	bool bIsSpellMage = false;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	bool bCanBeAttacked = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	bool bIsMoveWarrior = false;
	
	// ----------------------------
	// --     Movement Output    --
	// ----------------------------
	
	UPROPERTY( VisibleAnywhere, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<FIntPoint> Path = {FIntPoint(-999,-999)};
	
	UPROPERTY( VisibleAnywhere,  Category = "Grid", meta = (AllowPrivate = "true"))
	TMap<FIntPoint, FPathData> PathData;
	
	// ----------------------------
	// --       Tiles Check      --
	// ----------------------------
	
	UPROPERTY( VisibleAnywhere,  Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<FIntPoint> DiscoverTileIndex = {FIntPoint(-999,-999)};
	
	UPROPERTY(	VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<int> DiscoverTileSortingCost = {0};
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	FPathData CurrentDiscoverTile;
	
	UPROPERTY(	VisibleAnywhere,  Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<FIntPoint> AnalyseTileIndex = {FIntPoint(-999,-999)};
	
	// ----------------------------
	// --       Neighbours       --
	// ----------------------------
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<FPathData> CurrentsNeighbors;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	FPathData CurrentNeighbors;
	
	// UFUNCTIONS
public:	
	// ----------------------------
	// --       Constructor      --
	// ----------------------------
	
	UGridPath();
	
	// ----------------------------
	// --        Overrides       --
	// ----------------------------
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// ----------------------------
	// --      Height Checks     --
	// ----------------------------

	bool IsValidHeight(FDC_TileData* IndexTestData, FDC_TileData* CurrentIndexData);

	bool IsValidHeightWarrior(FDC_TileData* IndexTestData, FDC_TileData* CurrentIndexData);
	
	// ----------------------------
	// --     Find Neighbours    --
	// ----------------------------

	UFUNCTION()
	TArray<FIntPoint> FindTileNeighbours(FIntPoint Index);
	
	// ----------------------------
	// --  Generate Pathfinding  --
	// ----------------------------

	/**
	 * @param Start Starting Position
	 * @param End Ending Position
	 * @param bReachable if we don't need to reach the end
	 * @param PathLenght Lenght of the Path
	 * @param bSpellMage if is a spell mage
	 * @param bCanAttacked if it can be attacked
	 * @param bMoveWarrior if is a warrior movement
	 * @return 
	 */
	UFUNCTION()
	TArray<FIntPoint> FindPath(FIntPoint Start, FIntPoint End = FIntPoint(-999,-999) , bool bReachable, int PathLenght, bool bSpellMage, bool bCanAttacked, bool bMoveWarrior);


	TArray<FIntPoint> NewFindPath(FIntPoint Start, ACustomPlayerController* CPC);
	
	// ---------------------------- //
	// --         SETTERS        -- //
	// ---------------------------- //

	UFUNCTION()
	void SetGrid(AGrid* GridRef);

protected:
	// ----------------------------
	// --        Overrides       --
	// ----------------------------
	
	virtual void BeginPlay() override;
	
	// ----------------------------
	// --          Path          --
	// ----------------------------

	UFUNCTION()
	TArray<FIntPoint> GeneratePath();

	UFUNCTION()
	void ClearGeneratedPath();
	
	// ----------------------------
	// -  Tile Discovery (Set-up) -
	// ----------------------------

	UFUNCTION()
	void DiscoverTile(FPathData TilePath);

	UFUNCTION()
	int MinimalCostBetweenTwoTile(FIntPoint Index1, FIntPoint Index2);

	UFUNCTION()
	void InsertTileDiscoverList(FPathData TilePath);

	UFUNCTION()
	bool IsInputDataValid(FIntPoint Start, FIntPoint End);
	
	// ----------------------------
	// - Tile Discovery (Process) -
	// ----------------------------

	UFUNCTION()
	FPathData PullCheapestTileOutOfDiscoverList();

	UFUNCTION()
	bool AnalyseNextDiscoverTile();
	
	// ----------------------------
	// --        Neighbours      --
	// ----------------------------

	UFUNCTION()
	bool DiscoverNextNeighbours();
	
	// Test validity of Tile for walkable
	UFUNCTION()
	TArray<FPathData> GetValidTileNeighbours(FIntPoint Index);
};

