// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spawner.generated.h"

class AGrid;
UCLASS()
class DIVINECONFLICT_API ASpawner : public AActor
{
	GENERATED_BODY()

	// UPROPERTIES
public:	
	// ----------------------------
	// Components

	UPROPERTY(VisibleAnywhere, Category = "Spawner", meta = (AllowPrivate = "true"))
	TObjectPtr<UStaticMeshComponent> SpawnerMesh;
	
	// ----------------------------
	// References

	UPROPERTY(EditAnywhere, Category = "Spawner")
	TObjectPtr<AGrid> Grid;

protected:
	// ----------------------------
	// Position in Grid
	
	UPROPERTY()
	FIntPoint GridPosition = FIntPoint(0, 0);

	// UFUNCTIONS
public:
	// ----------------------------
	// Constructor
	
	ASpawner();
	
	// ----------------------------
	// Override
	virtual void Tick(float DeltaTime) override;

	// ----------------------------
	// Getter

	UFUNCTION()
	FIntPoint GetGridPosition();
	
	// ----------------------------
	// Setter
	
	UFUNCTION()
	void SetGridPosition(FIntPoint Position);

protected:
	// ----------------------------
	// Override
	
	virtual void BeginPlay() override;
	
};
