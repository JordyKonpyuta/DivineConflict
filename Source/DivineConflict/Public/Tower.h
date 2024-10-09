// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumsList.h"
#include "Grid.h"
#include "GridVisual.h"
#include "GameFramework/Actor.h"
#include "Tower.generated.h"

class AUnit;
class AGrid;

UCLASS()
class DIVINECONFLICT_API ATower : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATower();

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* Mesh;

	UFUNCTION(BlueprintNativeEvent)
	void SetMesh();

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Unit")
	AUnit* UnitInGarrison = nullptr;
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Bool")
	bool IsGarrisoned = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	AGrid* Grid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	TArray<FIntPoint> TilesInRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controller")
	ACustomPlayerController* PlayerController;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Stats")
	int Attack = 5;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Bool")
	bool CanAttack = true;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Enum")
	EPlayer PlayerOwner = EPlayer::P_Neutral;
	

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Grid")
	FIntPoint GridPosition = FIntPoint(-999, -999);





public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Replicated)
	AUnit* UnitToAttack = nullptr;

	UPROPERTY(Replicated)
	FIntPoint UnitToAttackPosition = FIntPoint(-999, -999);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bool")
	bool IsSelected = false;
	
	UFUNCTION(BlueprintCallable)
	int GetAttack();
	UFUNCTION(BlueprintCallable)
	void SetAttack(int NewAttack);
	UFUNCTION(BlueprintCallable)
	EPlayer GetPlayerOwner();
	UFUNCTION(BlueprintCallable)
	void SetPlayerOwner(EPlayer NewPlayerOwner);
	UFUNCTION(BlueprintCallable)
	bool GetCanAttack();
	UFUNCTION(BlueprintCallable)
	void SetCanAttack(bool NewCanAttack);

	UFUNCTION(BlueprintCallable)
	void AttackUnit();
	UFUNCTION(BlueprintCallable)
	void UpdateVisuals();

	FIntPoint GetGridPosition();

	void SetGridPosition(FIntPoint NewGridPosition);

	UFUNCTION()
	void PreprareAttack(AUnit* UnitAttack);
};
