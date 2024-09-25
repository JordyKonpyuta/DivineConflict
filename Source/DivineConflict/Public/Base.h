// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomPlayerController.h"
#include "GameFramework/Actor.h"
#include "EnumsList.h"
#include "Base.generated.h"

class APlayerController;
class ACustomPlayerState;
class AGrid;

UCLASS()
class DIVINECONFLICT_API ABase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABase();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	AGrid* Grid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* Mesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Stats
	UPROPERTY()
	int Health = 200;

	UPROPERTY()
	bool IsHell = false;

	UPROPERTY()
	int GoldCostUpgrade = 20;

	UPROPERTY()
	int StoneCostUpgrade = 20;

	UPROPERTY()
	int WoodCostUpgrade = 20;

	UPROPERTY()
	FIntPoint GridPosition= FIntPoint(-999,-999);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mesh")
	class UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ref", meta = (AllowPrivate = "true"))
	ACustomPlayerState* PlayerStateRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Controller")
	ACustomPlayerController* PlayerControllerRef;

	

	// Getter
	UFUNCTION(BlueprintCallable)
	int GetHealth();
	UFUNCTION(BlueprintCallable)
	bool GetIsHell();
	UFUNCTION(BlueprintCallable)
	int GetGoldCostUpgrade();
	UFUNCTION(BlueprintCallable)
	int GetStoneCostUpgrade();
	UFUNCTION(BlueprintCallable)
	int GetWoodCostUpgrade();
	UFUNCTION(BlueprintCallable)
	FIntPoint GetGridPosition();
	
	// Setter
	UFUNCTION(BlueprintCallable)
	void SetHealth(int h);
	UFUNCTION(BlueprintCallable)
	void SetIsHell(bool bH);
	UFUNCTION(BlueprintCallable)
	void SetCostsUpgrade(int g, int s, int w);
	UFUNCTION(BlueprintCallable)
	void SetGridPosition(FIntPoint GridP);

	UFUNCTION(BlueprintCallable)
	void CheckIfDead();
	UFUNCTION(BlueprintNativeEvent)
	void SetMesh();

	// FUNCTIONS FOR UI
	UFUNCTION(BlueprintNativeEvent)
	void OnDeath();

	UFUNCTION(BlueprintCallable)
	void TakeDamage(int Damage);

	UFUNCTION(BlueprintCallable)
	void Upgrade();
};
