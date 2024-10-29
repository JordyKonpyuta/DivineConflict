// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumsList.h"
#include "GameFramework/Pawn.h"
#include "GhostUnitSpawning.generated.h"

class UStaticMeshComponent;

UCLASS()
class DIVINECONFLICT_API AGhostUnitSpawning : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGhostUnitSpawning();

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> MaterialToGhosts;

	UPROPERTY()
	TArray<TObjectPtr<UStaticMesh>> MeshUnits;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	EUnitType UnitType;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	EUnitType GetUnitType();

	UFUNCTION()
	void SetUnitType(EUnitType Type);

	UFUNCTION()
	void Spawn();

};
