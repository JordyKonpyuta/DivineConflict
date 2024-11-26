// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnumsList.h"
#include "InteractInterface.h"
#include "Components/WidgetComponent.h"
#include "Unit.generated.h"


class UNiagaraComponent;
class UNiagaraSystem;
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
	TObjectPtr<UWidgetComponent> DamageWidgetComponent;
	
	// ----------------------------
	// Materials

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInterface>> AllMaterials;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> MaterialToGhosts;
	
	// ----------------------------
	// Niagaras
	
	UPROPERTY(Replicated)
	TObjectPtr<UNiagaraSystem> BuffTankSys_NS;
	
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> BuffTankComp_NS;
	
	// ----------------------------
	// References
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly ,Category = "Unit")
	TObjectPtr<AGrid> Grid;
	
	UPROPERTY()
	TObjectPtr<AUnit> UnitToAttackRef;
	
	UPROPERTY()
	TObjectPtr<ABuilding> BuildingToAttackRef;
	
	UPROPERTY()
	TObjectPtr<ABase> EnemyBase;

	// ----------------------------
	// Enums

	UPROPERTY(BlueprintReadOnly)
	EUnitName UnitName;

	// ----------------------------
	// Timer Handles

	FTimerHandle MoveTimerHandle;
	FTimerDelegate MoveTimerDelegate;

	FTimerHandle UnitAttackAnimTimer;
	FTimerDelegate UnitAttackAnimDelegate;

	FTimerHandle InitializationTimer;

	// ----------------------------
	// Booleans
	
	UPROPERTY()
	bool UsedSpecial;

	UPROPERTY(Replicated)
	bool bJustBecameGarrison = false;

	UPROPERTY(Blueprintable, BlueprintReadOnly,Replicated)
	bool HasMoved = false;

	UPROPERTY(Blueprintable, BlueprintReadOnly, Replicated)
	bool HasActed = false;

	UPROPERTY(Blueprintable, BlueprintReadOnly, Replicated)
	bool FirstActionIsMove = false;
	
	UPROPERTY(Blueprintable, BlueprintReadOnly, Replicated)
	bool bHasClimbed = false;

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
	TArray<FIntPoint> FutureMovementWithSpecial;
	
	UPROPERTY(Replicated)
	TArray<FIntPoint> PathToCross;

	// ----------------------------
	// Movement General

	FVector UnitNewLocation;

	// ----------------------------
	// Unit Spawn FIntPoints
	UPROPERTY(VisibleAnywhere , BlueprintReadOnly, Category = "UnitType")
	TArray<int> UnitCost;

	// ----------------------------
	// Textures

	UPROPERTY()
	TObjectPtr<UTexture2D> HeavenIcon;
	UPROPERTY()
	TObjectPtr<UTexture2D> HellIcon;
	UPROPERTY()
	TObjectPtr<UTexture2D> NeutralIcon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Texture")
	TObjectPtr<UTexture2D> UnitIcon;

	FTimerHandle IconTimerHandle;

	// ----------------------------
	// Death

	FTimerHandle DeathTimerHandle;

	FRotator UnitRotation = FRotator(0,0,0);

	// ----------------------------
	// ----------------------------
	
protected:
	
	// ----------------------------
	// Meshes

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Unit", Replicated, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> UnitMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
	TObjectPtr<UStaticMeshComponent> GhostsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
	TObjectPtr<UStaticMeshComponent> GhostsFinaleLocationMesh;
	
	// ----------------------------
	// References
	
	UPROPERTY(Replicated)
	TObjectPtr<ACustomPlayerController> PlayerControllerRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
	TObjectPtr<ABuilding> BuildingRef = nullptr;

	UPROPERTY()
	TObjectPtr<ATower> TowerRef = nullptr;

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

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
	bool IsGarrison = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Unit")
	bool bBuffTank = false;

	UPROPERTY(Replicated)
	bool bIsGhosts = false;

	UPROPERTY(Replicated)
	bool bIsCommandeerBuffed = false;

	UPROPERTY()
	bool bBeganAttack = false;
	
	bool WillMove = true;
	
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

	virtual void DisplayWidgetTutorial();
	
	// ----------------------------
	// Interactions
	
	UFUNCTION()
	void interaction(ACustomPlayerController *PlayerController);
	
	// ----------------------------
	// Turn Switch
	
	UFUNCTION()
	void NewTurn();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_NewTurn();
	
	// ----------------------------
	// Grid
	
	UFUNCTION(Server,Reliable)
	void Server_AddOnGrid();

	UFUNCTION(BlueprintCallable)
	void SetGrid(AGrid* NewGrid);

	// ----------------------------
	// Pre Movement

	UFUNCTION(NetMulticast, Reliable)
	void Multi_PrepareMove(const TArray<FIntPoint>& NewPos);
	
	// ----------------------------
	// Movement

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
	// Attack

	UFUNCTION()
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION(BlueprintCallable)
	void AttackUnit(AUnit* UnitToAttack);

	UFUNCTION(BlueprintCallable,Server,Reliable)
	void AttackBase(ABase* BaseToAttack);

	UFUNCTION(BlueprintCallable,Server,Reliable)
	void AttackBuilding(ABuilding* BuildingToAttack);
	
	UFUNCTION()
	void AnimAttack(AActor* ThingToAttack);

	// ----------------------------
	// Special

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	virtual void Special();
	
	virtual void SpecialUnit(AUnit* UnitToAttack);
	
	virtual void SpecialBase(ABase *BaseToAttack);
	
	// ----------------------------
	// Get Buffs

	UFUNCTION(Server, Reliable)
	void Server_GetBuffs();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_GetBuffs();

	UFUNCTION(Server, Reliable)
	void Server_GetBuffsVisual();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_GetBuffsVisual();
	
	// ----------------------------
	// Cancel Actions

	UFUNCTION(NetMulticast, Reliable)
	void Multi_CancelMove();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_CancelAttack();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_CancelSpecial();

	// ----------------------------
	// Textures
	
	UFUNCTION()
	void SetUnitIcon();

	// ----------------------------
	// Death

	UFUNCTION()
	void Server_DeathAnim();

	UFUNCTION()
	void Multi_DeathAnim();

	UFUNCTION(Server,Reliable)
	void Server_DestroyUnit();
	
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
	UFUNCTION()
	EPlayer GetPlayerOwner();

	// String
	UFUNCTION(BlueprintCallable,BlueprintPure)
	FString GetNameUnit();

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

	UFUNCTION(BlueprintCallable)
	void SetPlayerOwner(EPlayer po);

	UFUNCTION(NetMulticast,Reliable)
	void SetPlayerOwnerMulticast(EPlayer po);

	// Fstring
	UFUNCTION(BlueprintCallable)
	void SetName(FString n);

	// FIntPoint
	UFUNCTION()
	void SetIndexPosition(FIntPoint ip);
	
	// ----------------------------
	// ----------------------------
protected:
	// Overrides
	
	virtual void BeginPlay() override;
	
	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;

	//-----------------------------
	// Initialization

	UFUNCTION(Server, Reliable)
	void AssignPlayerController();
	
	// Movement

	UFUNCTION(Client,Reliable)
	void InitGhosts();

	UFUNCTION()
	void MoveGhosts(float DeltaTime);

	UFUNCTION()
	void MoveGhostsAction(float DeltaTime,const TArray<FIntPoint> &PathToFollowGhost);

	// ----------------------------
	// Initial tests
	
	UFUNCTION()
	void testOnTower();
	
	void SetGrid();
};
