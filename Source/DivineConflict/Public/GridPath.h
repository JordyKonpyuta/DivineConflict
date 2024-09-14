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
	FIntVector2 Index = FIntVector2(-999, -999);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PathData")
	int costEntrerToTile = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PathData")
	int MinCostToTarget = 9999999;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PathData")
	int CostFormStart = 9999999;
	UPROPERTY(EditAnywhere,  Category = "PathData")
	FIntVector2 PreviewsIndex = FIntVector2(-999, -999);
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
	FIntVector2 StartPoint = FIntVector2(-999	,-999);
	UPROPERTY( VisibleAnywhere,  Category = "Grid", meta = (AllowPrivate = "true"))
	FIntVector2 EndPoint = FIntVector2(-999, -999);
	UPROPERTY( VisibleAnywhere, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<FIntVector2> Path;
	UPROPERTY( VisibleAnywhere,  Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<FIntVector2> DiscoverTileIndex;
	UPROPERTY(	VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<int> DiscoverTileSortingCost;
	UPROPERTY(	VisibleAnywhere,  Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<FIntVector2> AnalyseTileIndex;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<FPathData> CurrentsNeighbors;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	FPathData CurrentDiscoverTile;
	UPROPERTY( VisibleAnywhere,  Category = "Grid", meta = (AllowPrivate = "true"))
	TMap<FIntVector2, FPathData> PathData;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	FPathData CurrentNeighbors;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	bool bIsReachable = false;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	int MaxLenght = 0;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	bool bIsFight = false;
	bool bIsEscalation = false;
	
	
	TArray<FIntVector2> FindTileNeighbors(FIntVector2 Index);
	bool IsInputDataValid(FIntVector2 Start, FIntVector2 End);
	void DiscoverTile(FPathData TilePath);
	int MinimulCostBetweenTwoTile(FIntVector2 Index1, FIntVector2 Index2);
	bool AnalyseNextDiscoverTile();
	TArray<FIntVector2> GeneratePath();
	FPathData PullCheapestTileOutOfDiscoverList();
	bool DiscoverNextNeighbors();
	TArray<FPathData> GetValidTileNeighbors(FIntVector2 Index);
	void InserTileDiscoverList(FPathData TilePath);
	void ClearGeneratedPath();
	bool IsValidHeigh(FDC_TileData IndextestData, FDC_TileData CurrentIndexData);
	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	TArray<FIntVector2> FindPath(FIntVector2 Start, FIntVector2 End , bool IsReachable, int PathLenght, bool IsEscalation);

	
		
};

