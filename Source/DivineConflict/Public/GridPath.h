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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PathData")
	FVector2D Index = FVector2D(-999, -999);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PathData")
	int costEntrerToTile = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PathData")
	int MinCostToTarget = 9999999;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PathData")
	int CostFormStart = 9999999;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PathData")
	FVector2D PreviewsIndex = FVector2D(-999, -999);
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

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	FVector2D StartPoint = FVector2D(-999	,-999);
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	FVector2D EndPoint = FVector2D(-999, -999);
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<FVector2D> Path;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<FVector2D> DiscoverTileIndex;
	UPROPERTY(	VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<int> DiscoverTileSortingCost;
	UPROPERTY(	VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<FVector2D> AnalyseTileIndex;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TArray<FPathData> CurrentsNeighbors;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	FPathData CurrentDiscoverTile;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TMap<FVector2D, FPathData> PathData;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	FPathData CurrentNeighbors;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	bool bIsReachable = false;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	int MaxLenght = 0;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	bool bIsFight = false;
	bool bIsEscalation = false;
	
	
	TArray<FVector2D> FindTileNeighbors(FVector2D Index);
	bool IsInputDataValid(FVector2D Start, FVector2D End);
	void DiscoverTile(FPathData TilePath);
	int MinimulCostBetweenTwoTile(FVector2D Index1, FVector2D Index2);
	bool AnalyseNextDiscoverTile();
	TArray<FVector2D> GeneratePath();
	FPathData PullCheapestTileOutOfDiscoverList();
	bool DiscoverNextNeighbors();
	TArray<FPathData> GetValidTileNeighbors(FVector2D Index);
	void InserTileDiscoverList(FPathData TilePath);
	void ClearGeneratedPath();
	bool IsValidHeigh(FDC_TileData IndextestData, FDC_TileData CurrentIndexData);
	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION( BlueprintCallable)
	TArray<FVector2D> FindPath(FVector2D Start, FVector2D End , bool IsReachable, int PathLenght, bool IsEscalation);

	
		
};

