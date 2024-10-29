// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Obstacle.generated.h"

class AGrid;

UCLASS()
class DIVINECONFLICT_API AObstacle : public AActor
{
	GENERATED_BODY()

	//UPROPERTIES
public:	
	// ----------------------------
	// Appearance
	
	UPROPERTY(BlueprintReadOnly, Category = "Unit", Replicated, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> Scene;
	
	UPROPERTY(BlueprintReadOnly, Category = "Unit", Replicated, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Plane;
	
	// ----------------------------
	// References
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	TObjectPtr<AGrid> GridRef;
	
protected:

	// UFUNCTIONS
public:
	// ----------------------------
	// Constructor
	
	AObstacle();
	
	// ----------------------------
	// Overwrites
	
	virtual void Tick(float DeltaTime) override;

protected:
	// ----------------------------
	// Overwrites
	
	virtual void BeginPlay() override;
};