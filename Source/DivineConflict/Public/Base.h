// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnumsList.h"
#include "Base.generated.h"

class ACustomPlayerController;
class ACustomPlayerState;
class AGrid;
class AUnit;

UCLASS()
class DIVINECONFLICT_API ABase : public AActor
{
	GENERATED_BODY()

	// UPROPERTIES
public:	
	// Sets default values for this actor's properties
	ABase();
	
	// ----------------------------
	// Components

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* Mesh;
	
	// ----------------------------
	// References
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	AGrid* Grid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ref", meta = (AllowPrivate = "true"))
	ACustomPlayerState* PlayerStateRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Controller")
	ACustomPlayerController* PlayerControllerRef;
	
	// ----------------------------
	// Booleans

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bool")
	bool IsSelected = false;

	// ----------------------------
	// Grid Indexes
	
	UPROPERTY(Blueprintable,Replicated, BlueprintReadOnly, Category = "Grid")
	TArray<FIntPoint> AllSpawnLoc;

	// ----------------------------
	// Enums
	
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Enum")
	EPlayer PlayerOwner = EPlayer::P_Neutral;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ----------------------------
	// References

	UPROPERTY()
	AUnit* UnitSpawned;
	
	// ----------------------------
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

	// UFUNCTIONS
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// ----------------------------
	// Spawn Visuals
	
	UFUNCTION(BlueprintCallable)
	void VisualSpawn();
	
	// ----------------------------
	// Upgrade

	UFUNCTION(BlueprintCallable)
	void Upgrade();
	
	// ----------------------------
	// Prepare Actions
	
	UFUNCTION()
	void BasePreAction(AUnit* UnitSp);

	// ----------------------------
	// Action
	
	UFUNCTION()
	void BaseAction();

	// ----------------------------
	// Take Damage

	UFUNCTION(BlueprintCallable)
	void TakeDamage(int Damage);

	// ----------------------------
	// Check Death

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerCheckIfDead(int H);
	
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void MulticastCheckIfDead(int h);
	
	// ----------------------------
	// UI
	
	UFUNCTION(BlueprintNativeEvent)
	void OnDeath();

	// ----------------------------
	// GETTERS

	// Stats
	UFUNCTION(BlueprintCallable)
	int GetHealth();

	// Costs
	UFUNCTION(BlueprintCallable)
	int GetGoldCostUpgrade();
	
	UFUNCTION(BlueprintCallable)
	int GetStoneCostUpgrade();
	
	UFUNCTION(BlueprintCallable)
	int GetWoodCostUpgrade();

	// Index Position
	UFUNCTION(BlueprintCallable)
	FIntPoint GetGridPosition();

	// ----------------------------
	// SETTERS
	
	// Components
	UFUNCTION(BlueprintNativeEvent)
	void SetMesh();
	
	// Stats
	UFUNCTION(BlueprintCallable)
	void SetHealth(int h);

	// Player State
	UFUNCTION()
	void SetPlayerState();

	// Costs
	UFUNCTION(BlueprintCallable)
	void SetCostsUpgrade(int g, int s, int w);

	// Index Position
	UFUNCTION(BlueprintCallable)
	void SetGridPosition(FIntPoint GridP);

private:
};
