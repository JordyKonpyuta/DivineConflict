// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

public:	
	// Sets default values for this component's properties
	UGridPath();

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	AGrid* Grid = nullptr;

	


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY( VisibleAnywhere, Category = "Grid", meta = (AllowPrivate = "true"))
	FIntPoint StartPoint = FIntPoint(-999	,-999);
	UPROPERTY( VisibleAnywhere,  Category = "Grid", meta = (AllowPrivate = "true"))
	FIntPoint EndPoint = FIntPoint(-999, -999);
	UPROPERTY( VisibleAnywhere, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<FIntPoint> Path;
	UPROPERTY( VisibleAnywhere,  Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<FIntPoint> DiscoverTileIndex;
	UPROPERTY(	VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<int> DiscoverTileSortingCost;
	UPROPERTY(	VisibleAnywhere,  Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<FIntPoint> AnalyseTileIndex;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<FPathData> CurrentsNeighbors;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	FPathData CurrentDiscoverTile;
	UPROPERTY( VisibleAnywhere,  Category = "Grid", meta = (AllowPrivate = "true"))
	TMap<FIntPoint, FPathData> PathData;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	FPathData CurrentNeighbors;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	bool bIsReachable = false;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	int MaxLenght = 0;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	bool bIsFight = false;
	bool bIsEscalation = false;
	
	
	TArray<FIntPoint> FindTileNeighbors(FIntPoint Index);
	bool IsInputDataValid(FIntPoint Start, FIntPoint End);
	void DiscoverTile(FPathData TilePath);
	int MinimulCostBetweenTwoTile(FIntPoint Index1, FIntPoint Index2);
	bool AnalyseNextDiscoverTile();
	TArray<FIntPoint> GeneratePath();
	FPathData PullCheapestTileOutOfDiscoverList();
	bool DiscoverNextNeighbors();
	TArray<FPathData> GetValidTileNeighbors(FIntPoint Index);
	void InserTileDiscoverList(FPathData TilePath);
	void ClearGeneratedPath();
	
	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool IsValidHeigh(FDC_TileData* IndextestData, FDC_TileData* CurrentIndexData);
	
	UFUNCTION()
	TArray<FIntPoint> FindPath(FIntPoint Start, FIntPoint End , bool IsReachable, int PathLenght, bool IsEscalation);

	
		
};

