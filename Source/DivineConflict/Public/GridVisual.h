// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "F_DC_TileData.h"
#include "Components/ActorComponent.h"
#include "GridVisual.generated.h"

class AGrid;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DIVINECONFLICT_API UGridVisual : public UActorComponent
{
	GENERATED_BODY()

	// UPROPERTIES
public:	

protected:
	// ----------------------------
	// --       References       --
	// ----------------------------
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivate = "true"))
	TObjectPtr<AGrid> Grid = nullptr;	

	// UFUNCTIONS
public:	
	// ----------------------------
	// --       Constructor      --
	// ----------------------------
	
	UGridVisual();
	
	// ----------------------------
	// --        Overrides       --
	// ----------------------------
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// ----------------------------
	// --     State Modifier     --
	// ----------------------------
	
	UFUNCTION(BlueprintCallable, Blueprintable)
	void AddStateToTile(FIntPoint Index, EDC_TileState TileState);
	
	UFUNCTION(BlueprintCallable)
	void RemoveStateFromTile(FIntPoint Index, EDC_TileState TileState);
	
	UFUNCTION(BlueprintCallable)
	void RemoveAllStateFromTile(FIntPoint Index);
	
	// ---------------------------- //
	// --        SETTERS         -- //
	// ---------------------------- //

	UFUNCTION()
	void SetGrid(AGrid* GridRef);

protected:
	// ----------------------------
	// --        Overrides       --
	// ----------------------------
	
	virtual void BeginPlay() override;
	
	// ----------------------------
	// --       Get Colors       --
	// ----------------------------
	
	UFUNCTION()
	FLinearColor GetColorFromState(TArray<EDC_TileState> TileState);
	
	// ----------------------------
	// --      Update Colors     --
	// ----------------------------

	UFUNCTION(BlueprintCallable	)
	void UpdateVisuals(FIntPoint Index);
		
};
