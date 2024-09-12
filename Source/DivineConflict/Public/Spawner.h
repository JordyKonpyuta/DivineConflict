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

	UPROPERTY(VisibleAnywhere, Category = "Spawner", meta = (AllowPrivate = "true"))
	UStaticMeshComponent* SpawnerMesh;
	
public:	
	// Sets default values for this actor's properties
	ASpawner();

	UPROPERTY(EditAnywhere, Category = "Spawner")
	AGrid* Grid;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	FVector2D GridPosition;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	bool IsHell = false;

	UFUNCTION(BlueprintCallable)
	void SetGridPosition(FVector2D Position);

	UFUNCTION(BlueprintCallable)
	FVector2D GetGridPosition();

};
