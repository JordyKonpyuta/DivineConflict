// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "F_DC_TileData.h"
#include "Components/ActorComponent.h"
#include "CookOnTheSide/CookOnTheFlyServer.h"
#include "GridVisual.generated.h"

class AGrid;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DIVINECONFLICT_API UGridVisual : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGridVisual();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	AGrid* Grid;

	UFUNCTION()
	FLinearColor GetColorFromState(TArray<EDC_TileState> TileState);

	UFUNCTION(	)
	void UpdateVisuals(FIntPoint Index);	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetGrid(AGrid* GridRef);

	UFUNCTION()
	void addStateToTile(FIntPoint Index, EDC_TileState TileState);

	UFUNCTION()
	void RemoveStateFromTile(FIntPoint Index, EDC_TileState TileState);

	
	
	
		
};
