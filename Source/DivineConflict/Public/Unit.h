// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnumsList.h"
#include "InteractInterface.h"
#include "Unit.generated.h"


class ABuilding;
class ABase;
class AGrid;
class UStaticMeshComponent;
class ATower;
class ACustomPlayerController;

UENUM(BlueprintType)
enum class EUnitName : uint8
{
	Tank UMETA(DisplayName = "Tank"),
	Warrior UMETA(DisplayName = "Warrior"),
	Mage UMETA(DisplayName = "Mage"),
	Leader UMETA(DisplayName = "Leader"),
};


UCLASS()
class DIVINECONFLICT_API AUnit : public APawn , public IInteractInterface
{
	GENERATED_BODY()

	


public:
	// Sets default values for this pawn's properties
	AUnit();

	virtual bool Interact_Implementation(ACustomPlayerController* PlayerController) override;

	UPROPERTY( EditAnywhere, BlueprintReadOnly ,Category = "Unit")
	AGrid* Grid;

	UFUNCTION()
	void interaction(ACustomPlayerController *PlayerController);

	UFUNCTION(BlueprintCallable)
	void SetGrid(AGrid* NewGrid);

	UPROPERTY(BlueprintReadOnly)
	EUnitName UnitName;

	UPROPERTY()
	TArray<UMaterialInterface*> AllMaterials;

	UPROPERTY()
	UMaterialInterface* MaterialToGhosts;

	UFUNCTION()
	void Destroyed() override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Unit", Replicated, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* UnitMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
	UStaticMeshComponent* GhostsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
	UStaticMeshComponent* GhostsFinaleLocationMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "Unit")
	EPlayer PlayerOwner = EPlayer::P_Neutral;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Unit")
	bool bIsClimbing = false;
	
	UPROPERTY()
	ACustomPlayerController* PlayerControllerRef;
	
	void SetGrid();

	UFUNCTION()
	void testOnTower();
	

	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Texture")
	UTexture2D* UnitIconParadise;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Texture")
	UTexture2D* UnitIconHell;
	
	// Units stats
	UPROPERTY()
	int Attack = 0;
	
	UPROPERTY()
	int Defense = 0;
	
	UPROPERTY(Replicated,VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
	int CurrentHealth = 0;
	
	UPROPERTY()
	int MaxHealth = 0;
	
	UPROPERTY()
	int MovementCost = 0;
	
	UPROPERTY()
	int AttackCost = 0;
	
	UPROPERTY()
	int SpellCost = 0;

	UPROPERTY()
	bool IsSelected = false;

	UPROPERTY()
	bool IsHell = false;

	UPROPERTY()
	int TotalDamagesInflicted = 0;

	UPROPERTY(Replicated)
	bool IsGarrison = false;

	UPROPERTY()
	ABuilding* BuildingRef = nullptr;

	UPROPERTY()
	ATower* TowerRef = nullptr;

	UPROPERTY()
	FString Name = "";

	UPROPERTY(Replicated)
	int PM = 0;

	UPROPERTY()
	FIntPoint IndexPosition = FIntPoint(-999, -999);

	UPROPERTY()
	TArray<FIntPoint> Path;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Unit")
	bool bBuffTank = false;

	UPROPERTY(Replicated)
	bool bIsGhosts = false;
	
	UPROPERTY()
	float DeltaTimeGhosts = 0.0f;

	UPROPERTY()
	int CurrentIndexGhost = 0;

	UFUNCTION(Client,Reliable)
	void InitGhosts();

    UFUNCTION()
	void MoveGhosts(float DeltaTime);

	UFUNCTION(Server,Reliable)
	void Server_MoveGhosts(float DeltaTime ,const TArray<FIntPoint> &PathToFollowGhost);

	UFUNCTION(NetMulticast,Reliable)
	void MoveGhostsMulticast(float DeltaTime,const TArray<FIntPoint> &PathToFollowGhost);

public:
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere , BlueprintReadOnly, Category = "UnitType")
	TArray<int> UnitCost;

	UFUNCTION(Server,Reliable)
	void Server_AddOnGrid();

	UFUNCTION( BlueprintCallable,NetMulticast,Reliable)
	void Move(const TArray<FIntPoint> &PathToFollow);

	UFUNCTION()
	void MoveUnitEndTurn();

	UFUNCTION(NetMulticast,Reliable)
	void Multi_HiddeGhosts();

	UFUNCTION(Server,Reliable)
	void Server_Move(const TArray<FIntPoint> &PathToFollow);

	UFUNCTION()
	void TakeDamage(int Damage);

	UFUNCTION(BlueprintCallable)
	void AttackUnit();

	UFUNCTION(BlueprintCallable)
	void AttackBase(ABase* BaseToAttack);

	UFUNCTION(BlueprintCallable)
	virtual void Special();

	UFUNCTION(BlueprintCallable)
	bool GetIsGarrison();

	UFUNCTION(BlueprintCallable)
	void SetIsGarrison(bool bG);

	UFUNCTION(BlueprintCallable)
	ABuilding* GetBuildingRef();

	UFUNCTION(BlueprintCallable)
	void SetBuildingRef(ABuilding* Building);
	
	virtual void SpecialUnit(AUnit* UnitToAttack);

	UFUNCTION()
	UStaticMeshComponent* GetFinalGhostMesh();
	
	virtual void SpecialBase(ABase *BaseToAttack);

	UFUNCTION()
	void NewTurn();

	UPROPERTY(Replicated)
	TArray<FIntPoint> FutureMovement;

	UPROPERTY()
	FIntPoint FutureMovementPos;

	UPROPERTY()
	AUnit* UnitToAttackRef;
	
	UPROPERTY()
	ABuilding* BuildingToAttackRef;
	
	UPROPERTY()
	ABase* EnemyBase;

	UPROPERTY()
	bool UsedSpecial;

	UPROPERTY(Replicated)
	TArray<FIntPoint> PathToCross;

	UPROPERTY(Replicated)
	int PathToCrossPosition = 0;

	FTimerHandle MoveTimerHandle;

	UPROPERTY(Replicated)
	int MoveSequencePos = 0;

	UPROPERTY(Replicated)
	bool bJustBecameGarrison = false;

	UPROPERTY(Blueprintable, BlueprintReadOnly)
	bool HasMoved = false;

	UPROPERTY(Blueprintable, BlueprintReadOnly)
	bool HasActed = false;

	UFUNCTION()
	void InitializeFullMove(TArray <FIntPoint> FullMove);
	
	UFUNCTION(BlueprintCallable,NetMulticast,Reliable)
	void UnitMoveAnim();
	
	

	// Getter for units stats
	UFUNCTION(BlueprintCallable)
	int GetAttack();

	UFUNCTION(BlueprintCallable)
	int GetDefense();

	UFUNCTION(BlueprintCallable)
	int GetCurrentHealth();

	UFUNCTION(BlueprintCallable)
	int GetMaxHealth();

	UFUNCTION(BlueprintCallable)
	int GetMovementCost();

	UFUNCTION(BlueprintCallable)
	int GetAttackCost();

	UFUNCTION(BlueprintCallable)
	int GetSpellCost();

	UFUNCTION(BlueprintCallable)
	bool GetIsSelected();

	UFUNCTION(BlueprintCallable)
	bool GetIsHell();
	
	UFUNCTION(BluePrintCallable)
	EPlayer GetUnitTeam();

	UFUNCTION(BlueprintCallable)
	int GetTotalDamageInflicted();

	UFUNCTION(BlueprintCallable)
	FString GetName();

	UFUNCTION(BlueprintCallable)
	int GetPM();

	UFUNCTION()
	FIntPoint GetIndexPosition();

	UFUNCTION()
	EPlayer GetPlayerOwner();

	UFUNCTION()
	bool GetIsClimbing();

	UFUNCTION()
	bool GetBuffTank();

	// Setter for units stats
	UFUNCTION(BlueprintCallable)
	void SetAttack(int a);

	UFUNCTION(BlueprintCallable)
	void SetDefense(int d);

	UFUNCTION(BlueprintCallable)
	void SetCurrentHealth(int ch);

	UFUNCTION(BlueprintCallable)
	void SetMaxHealth(int mh);

	UFUNCTION(BlueprintCallable)
	void SetMovementCost(int mc);

	UFUNCTION(BlueprintCallable)
	void SetAttackCost(int ac);

	UFUNCTION(BlueprintCallable)
	void SetSpellCost(int sc);

	UFUNCTION(BlueprintCallable)
	void SetIsSelected(bool s);

	UFUNCTION(BlueprintCallable)
	void SetIsHell(bool h);

	UFUNCTION(BluePrintCallable)
	void SetUnitTeam(EPlayer PO);

	UFUNCTION(BlueprintCallable)
	void SetTotalDamageInflicted(int tdi);

	UFUNCTION(BlueprintCallable)
	void SetName(FString n);

	UFUNCTION(BlueprintCallable)
	void SetPM(int p);

	UFUNCTION()
	void SetIndexPosition(FIntPoint ip);

	UFUNCTION(BlueprintCallable)
	void SetPlayerOwner(EPlayer po);

	UFUNCTION()
	void SetIsClimbing(bool ic);

	UFUNCTION()
	void SetBuffTank(bool bt);

	UFUNCTION(NetMulticast,Reliable)
	void Multi_PrepareMove(const TArray<FIntPoint> &NewPos);

	UFUNCTION()
	void PrepareAttackUnit(FIntPoint AttackPos);

	UFUNCTION()
	void PrepareAttackBuilding(FIntPoint AttackPos);

	UFUNCTION()
	void PrepareAttackBase(FIntPoint AttackPos);

	UFUNCTION()
	void PrepareSpecial(FIntPoint SpecialPos);
};
