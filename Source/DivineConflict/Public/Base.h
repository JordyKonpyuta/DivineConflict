// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnumsList.h"
#include "Components/ArrowComponent.h"
#include "Base.generated.h"

class ACustomPlayerController;
class ACustomPlayerState;
class AGrid;
class AUnit;

UCLASS()
class DIVINECONFLICT_API ABase : public AActor
{
	GENERATED_BODY()

	// UPROPERTIES //
public:	
	// ----------------------------
	// --       Components       --
	// ----------------------------

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

	UPROPERTY()
	TObjectPtr<UArrowComponent> Arrow = nullptr;
	
	// ----------------------------
	// --       References       --
	// ----------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	TObjectPtr<AGrid> Grid = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Ref", meta = (AllowPrivate = "true"))
	TObjectPtr<ACustomPlayerState> PlayerStateRef = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Controller")
	TObjectPtr<ACustomPlayerController> PlayerControllerRef = nullptr;
	
	// ----------------------------
	// --        Booleans        --
	// ----------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bool")
	bool IsSelected = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bool")
	bool CanSpawn = true;

	// ----------------------------
	// --      Index (Grid)      --
	// ----------------------------
	
	UPROPERTY(Blueprintable,Replicated, BlueprintReadOnly, Category = "Grid")
	TArray<FIntPoint> AllSpawnLoc;

	// ----------------------------
	// --          Enums         --
	// ----------------------------
	
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Enum")
	EPlayer PlayerOwner = EPlayer::P_Neutral;

	// ----------------------------
	// --         Levels         --
	// ----------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int MaxLevel = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Stats")
	int Level = 1;

protected:
	// ----------------------------
	// --       References       --
	// ----------------------------

	UPROPERTY()
	TObjectPtr<AUnit> UnitSpawned;
	
	// ----------------------------
	// --          Stats         --
	// ----------------------------

		// Health
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Stats")
	int Health = 50;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Stats")
	int MaxHealth = Health;

		// Costs
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Stats")
	int GoldCostUpgrade = 20;
	
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Stats")
	int StoneCostUpgrade = 20;
	
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Stats")
	int WoodCostUpgrade = 20;

		// Position	
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Grid")
	FIntPoint GridPosition= FIntPoint(-999,-999);

	// ----------------------------
	// --         Timers         --
	// ----------------------------
	
	FTimerHandle PlayerStateHandle;
	
	// UFUNCTIONS //
public:	
	// ----------------------------
	// --       Constructor      --
	// ----------------------------
	
	ABase();
	
	// ----------------------------
	// --        Override        --
	// ----------------------------
	
	virtual void Tick(float DeltaTime) override;
	
	// ----------------------------
	// --     Spawn Visuals      --
	// ----------------------------
	
	UFUNCTION(BlueprintCallable)
	void VisualSpawn();
	
	// ----------------------------
	// --         Upgrade        --
	// ----------------------------

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_Upgrade();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void Multi_Upgrade();
	
	// ----------------------------
	// --      Damage Taken      --
	// ----------------------------
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// ----------------------------
	// --       Check Death      --
	// ----------------------------

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerCheckIfDead(int H);
	
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void MulticastCheckIfDead(int h);
	
	// ----------------------------
	// --           UI           --
	// ----------------------------
	
	UFUNCTION(BlueprintNativeEvent)
	void OnDeath();

	// ----------------------------
	// --       Tiles Check      --
	// ----------------------------
	
	UFUNCTION(BlueprintCallable)
	bool CheckTiles();

	// ---------------------------- //
	// --         GETTERS        -- //
	// ---------------------------- //

		// Stats
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetHealth();

		// Costs
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetGoldCostUpgrade();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetStoneCostUpgrade();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetWoodCostUpgrade();

		// Index Position
	UFUNCTION(BlueprintCallable)
	FIntPoint GetGridPosition();

	// ---------------------------- //
	// --         SETTERS        -- //
	// ---------------------------- //
	
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
	// ----------------------------
	// --        Override        --
	// ----------------------------

	virtual void BeginPlay() override;
	
	// ----------------------------
	// --         Timers         --
	// ----------------------------
	
	UFUNCTION()
	void TimerBeginPlay();

	UFUNCTION()
	void AssignPlayerState();
};
