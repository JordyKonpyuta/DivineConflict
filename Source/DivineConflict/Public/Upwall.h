// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Upwall.generated.h"

class AGrid;
class ACustomPlayerController;

UCLASS()
class DIVINECONFLICT_API AUpwall : public AActor
{
	GENERATED_BODY()

	// UPROPERTIES
public:
	// ----------------------------
	// --       Appearance       --
	// ----------------------------
	
	UPROPERTY(BlueprintReadOnly, Category = "Unit", Replicated, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> Scene = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit", Replicated, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Plane1 = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit", Replicated, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Plane2 = nullptr;
	
	// ----------------------------
	// --       References       --
	// ----------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	TObjectPtr<AGrid> GridRef = nullptr;
	
protected:
	// ----------------------------
	// --        Positions       --
	// ----------------------------
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Grid")
	FIntPoint GridPosition = FIntPoint(-999, -999);
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Grid")
	FIntPoint ClimbLocation = FIntPoint(-999, -999);
	
	// UFUNCTIONS
public:	
	// ----------------------------
	// --      Constructor       --
	// ----------------------------
	
	AUpwall();
	// ----------------------------
	// --        Overrides       --
	// ----------------------------
	
	virtual void Tick(float DeltaTime) override;
	
	// ---------------------------- //
	// --        GETTERS         -- //
	// ---------------------------- //

	UFUNCTION()
	FIntPoint GetGridPosition();

	UFUNCTION()
	FIntPoint GetClimbLocation();
	
	// ---------------------------- //
	// --        SETTERS         -- //
	// ---------------------------- //

	UFUNCTION()
	void SetGridPosition(FIntPoint NewGridPosition);

	UFUNCTION()
	void SetClimbLocation(FIntPoint NewClimbLocation);
	
protected:
	// ----------------------------
	// --        Overrides       --
	// ----------------------------
	
	virtual void BeginPlay() override;
};
