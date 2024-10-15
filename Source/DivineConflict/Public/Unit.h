// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnumsList.h"
#include "InteractInterface.h"
#include "Components/WidgetComponent.h"
#include "Unit.generated.h"


class UWidgetDamage2;
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

	

	// UPROPERTIES
public:
	// Sets default values for this pawn's properties
	AUnit();
	
	// ----------------------------
	// Components

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widget")
	UWidgetComponent* DamageWidgetComponent;
	
	// ----------------------------
	// Materials

	UPROPERTY()
	TArray<UMaterialInterface*> AllMaterials;

	UPROPERTY()
	UMaterialInterface* MaterialToGhosts;
	
	// ----------------------------
	// References
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly ,Category = "Unit")
	AGrid* Grid;
	
	UPROPERTY()
	AUnit* UnitToAttackRef;
	
	UPROPERTY()
	ABuilding* BuildingToAttackRef;
	
	UPROPERTY()
	ABase* EnemyBase;

	// ----------------------------
	// Enums

	UPROPERTY(BlueprintReadOnly)
	EUnitName UnitName;

	// ----------------------------
	// Timer Handles

	FTimerHandle MoveTimerHandle;

	FTimerHandle UnitAttackAnimTimer;

	// ----------------------------
	// Booleans
	
	UPROPERTY()
	bool UsedSpecial;

	UPROPERTY(Replicated)
	bool bJustBecameGarrison = false;

	UPROPERTY(Blueprintable, BlueprintReadOnly)
	bool HasMoved = false;

	UPROPERTY(Blueprintable, BlueprintReadOnly)
	bool HasActed = false;

	// ----------------------------
	// Ints
	
	UPROPERTY(Replicated)
	int PathToCrossPosition = 0;

	UPROPERTY(Replicated)
	int MoveSequencePos = 0;

	// ----------------------------
	// Movement FIntPoints

	UPROPERTY()
	FIntPoint FutureMovementPos;
	
	UPROPERTY(Replicated)
	TArray<FIntPoint> FutureMovement;
	
	UPROPERTY(Replicated)
	TArray<FIntPoint> PathToCross;
	

	// ----------------------------
	// Unit Spawn FIntPoints
	UPROPERTY(VisibleAnywhere , BlueprintReadOnly, Category = "UnitType")
	TArray<int> UnitCost;
	
	// ----------------------------
	// ----------------------------
protected:
	// Overrides
	
	virtual void BeginPlay() override;
	
	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;
	
	// ----------------------------
	// Meshes

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Unit", Replicated, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* UnitMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
	UStaticMeshComponent* GhostsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
	UStaticMeshComponent* GhostsFinaleLocationMesh;

	// ----------------------------
	// Textures
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Texture")
	UTexture2D* UnitIconParadise;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Texture")
	UTexture2D* UnitIconHell;
	
	// ----------------------------
	// References
	
	UPROPERTY()
	ACustomPlayerController* PlayerControllerRef;

	UPROPERTY()
	ABuilding* BuildingRef = nullptr;

	UPROPERTY()
	ATower* TowerRef = nullptr;

	// ----------------------------
	// Enums
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "Unit")
	EPlayer PlayerOwner = EPlayer::P_Neutral;

	// ----------------------------
	// Booleans
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Unit")
	bool bIsClimbing = false;

	UPROPERTY()
	bool IsSelected = false;

	UPROPERTY(Replicated)
	bool IsGarrison = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Unit")
	bool bBuffTank = false;

	UPROPERTY(Replicated)
	bool bIsGhosts = false;

	UPROPERTY()
	bool bIsCommandeerBuffed = false;

	UPROPERTY()
	bool bBeganAttack = false;
	
	// ----------------------------
	// Units stats
	
	UPROPERTY(Replicated,VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
	int CurrentHealth = 0;
	
	UPROPERTY()
	int MaxHealth = 0;

	UPROPERTY(Replicated)
	int PM = 0;
	
	UPROPERTY()
	int Attack = 0;
	
	UPROPERTY()
	int Defense = 0;

	UPROPERTY()
	FString Name = "";

	UPROPERTY()
	FVector UnitLocationInWorld;

	// ----------------------------
	// Position in Grid

	UPROPERTY()
	FIntPoint IndexPosition = FIntPoint(-999, -999);

	UPROPERTY()
	TArray<FIntPoint> Path;

	// ----------------------------
	// Ghosts
	
	UPROPERTY()
	float DeltaTimeGhosts = 0.0f;

	UPROPERTY()
	int CurrentIndexGhost = 0;

	
	// ----------------------------
	// ----------------------------
	// ----------------------------
	// UFUNCTIONS
public:

	// Overrides
	
	UFUNCTION()
	void Destroyed() override;
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual bool Interact_Implementation(ACustomPlayerController* PlayerController) override;
	
	// ----------------------------
	// Interactions
	
	UFUNCTION()
	void interaction(ACustomPlayerController *PlayerController);
	
	// ----------------------------
	// Turn Switch
	
	UFUNCTION()
	void NewTurn();
	
	// ----------------------------
	// Grid
	
	UFUNCTION(Server,Reliable)
	void Server_AddOnGrid();

	UFUNCTION(BlueprintCallable)
	void SetGrid(AGrid* NewGrid);

	// ----------------------------
	// Prepare Moves
	
	UFUNCTION(NetMulticast,Reliable)
	void Multi_PrepareMove(const TArray<FIntPoint> &NewPos);
	
	// ----------------------------
	// Movement

	UFUNCTION( BlueprintCallable,NetMulticast,Reliable)
	void Move(const TArray<FIntPoint> &PathToFollow);
	
	UFUNCTION(Server,Reliable)
	void Server_Move(const TArray<FIntPoint> &PathToFollow);

	UFUNCTION()
	void InitializeFullMove(TArray <FIntPoint> FullMove);
	
	UFUNCTION(BlueprintCallable,NetMulticast,Reliable)
	void UnitMoveAnim();
	
	UFUNCTION()
	virtual void MoveUnitEndTurn();

	// ----------------------------
	// Ghosts

	UFUNCTION(NetMulticast,Reliable)
	void Multi_HiddeGhosts();
	
	UFUNCTION()
	UStaticMeshComponent* GetFinalGhostMesh();

	// ----------------------------
	// Prepare Attacks
	
	UFUNCTION()
	void PrepareAttackUnit(FIntPoint AttackPos);

	UFUNCTION()
	void PrepareAttackBuilding(FIntPoint AttackPos);

	UFUNCTION()
	void PrepareAttackBase(FIntPoint AttackPos);

	// ----------------------------
	// Attack

	UFUNCTION()
	void TakeDamage(int Damage);

	UFUNCTION(BlueprintCallable)
	void AttackUnit(AUnit* UnitToAttack);

	UFUNCTION(BlueprintCallable,Server,Reliable)
	void AttackBase(ABase* BaseToAttack);
	
	UFUNCTION()
	void AnimAttack();
	
	// ----------------------------
	// Prepare Specials

	UFUNCTION()
	virtual void PrepareSpecial(FIntPoint SpecialPos);

	// ----------------------------
	// Special

	UFUNCTION(BlueprintCallable)
	virtual void Special();
	
	virtual void SpecialUnit(AUnit* UnitToAttack);
	
	virtual void SpecialBase(ABase *BaseToAttack);


	
	
	// ----------------------------
	// GETTERS //

	// StaticMesh
	UFUNCTION()
	UStaticMeshComponent* GetStaticMesh();
	
	// References
	UFUNCTION(BlueprintCallable)
	ABuilding* GetBuildingRef();
	
	// Int
	UFUNCTION(BlueprintCallable)
	int GetMaxHealth();
	
	UFUNCTION(BlueprintCallable)
	int GetCurrentHealth();

	UFUNCTION(BlueprintCallable)
	int GetPM();
	
	UFUNCTION(BlueprintCallable)
	int GetAttack();

	UFUNCTION(BlueprintCallable)
	int GetDefense();

	// Bool
	UFUNCTION(BlueprintCallable)
	bool GetIsSelected();

	UFUNCTION()
	bool GetIsClimbing();

	UFUNCTION()
	bool GetBuffTank();

	UFUNCTION(BlueprintCallable)
	bool GetIsGarrison();

	UFUNCTION()
	bool GetIsCommandeerBuffed();

	// Enums
	UFUNCTION(BluePrintCallable)
	EPlayer GetUnitTeam();

	UFUNCTION()
	EPlayer GetPlayerOwner();

	// String
	UFUNCTION(BlueprintCallable)
	FString GetName();

	// FIntPoint
	UFUNCTION()
	FIntPoint GetIndexPosition();

	// ----------------------------
	// SETTERS //

	// References
	UFUNCTION(BlueprintCallable)
	void SetBuildingRef(ABuilding* Building);
	
	// Int
	UFUNCTION(BlueprintCallable)
	void SetMaxHealth(int mh);
	
	UFUNCTION(BlueprintCallable)
	void SetCurrentHealth(int ch);
	
	UFUNCTION(BlueprintCallable)
	void SetPM(int p);
	
	UFUNCTION(BlueprintCallable)
	void SetAttack(int a);

	UFUNCTION(BlueprintCallable)
	void SetDefense(int d);
	
	// Bool
	UFUNCTION(BlueprintCallable)
	void SetIsSelected(bool s);
	
	UFUNCTION(BlueprintCallable)
	void SetIsGarrison(bool bG);

	UFUNCTION()
	void SetIsClimbing(bool ic);

	UFUNCTION()
	void SetBuffTank(bool bt);

	UFUNCTION()
	void SetIsCommandeerBuffed(bool bC);

	// Enums
	UFUNCTION(BluePrintCallable)
	void SetUnitTeam(EPlayer PO);

	UFUNCTION(BlueprintCallable)
	void SetPlayerOwner(EPlayer po);

	// Fstring
	UFUNCTION(BlueprintCallable)
	void SetName(FString n);

	// FIntPoint
	UFUNCTION()
	void SetIndexPosition(FIntPoint ip);
	
	// ----------------------------
	// ----------------------------
protected:

	// Movement

	UFUNCTION(Client,Reliable)
	void InitGhosts();

	UFUNCTION()
	void MoveGhosts(float DeltaTime);

	UFUNCTION(Server,Reliable)
	void Server_MoveGhosts(float DeltaTime ,const TArray<FIntPoint> &PathToFollowGhost);

	UFUNCTION(NetMulticast,Reliable)
	void MoveGhostsMulticast(float DeltaTime,const TArray<FIntPoint> &PathToFollowGhost);

	// ----------------------------
	// Initial tests
	
	UFUNCTION()
	void testOnTower();
	
	void SetGrid();
};
