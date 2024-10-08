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
class AUnit;

UCLASS()
class DIVINECONFLICT_API ABase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABase();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	AGrid* Grid;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* Mesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Stats
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Stats")
	int Health = 200;
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Stats")
	int GoldCostUpgrade = 20;
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Stats")
	int StoneCostUpgrade = 20;
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Stats")
	int WoodCostUpgrade = 20;
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Grid")
	FIntPoint GridPosition= FIntPoint(-999,-999);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ref", meta = (AllowPrivate = "true"))
	ACustomPlayerState* PlayerStateRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Controller")
	ACustomPlayerController* PlayerControllerRef;

	UFUNCTION(BlueprintCallable)
	void VisualSpawn();

	UPROPERTY(Blueprintable,Replicated, BlueprintReadOnly, Category = "Grid")
	TArray<FIntPoint> AllSpawnLoc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bool")
	bool IsSelected = false;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Enum")
	EPlayer PlayerOwner = EPlayer::P_Neutral;

	

	// Getter
	UFUNCTION(BlueprintCallable)
	int GetHealth();
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
	void SetCostsUpgrade(int g, int s, int w);
	UFUNCTION(BlueprintCallable)
	void SetGridPosition(FIntPoint GridP);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerCheckIfDead();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void MulticastCheckIfDead();
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
