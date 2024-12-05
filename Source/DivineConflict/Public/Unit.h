// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnumsList.h"
#include "InteractInterface.h"
#include "Components/WidgetComponent.h"
#include "Unit.generated.h"


class ACameraPlayer;
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
	None		UMETA(DisplayName = "None"),
	Tank		UMETA(DisplayName = "Tank"),
	Warrior		UMETA(DisplayName = "Warrior"),
	Mage		UMETA(DisplayName = "Mage"),
	Leader		UMETA(DisplayName = "Leader"),
};


UCLASS()
class DIVINECONFLICT_API AUnit : public APawn , public IInteractInterface
{
	GENERATED_BODY()

	

	// UPROPERTIES
public:
	// ----------------------------
	// --       Components       --
	// ----------------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widget")
	TObjectPtr<UWidgetComponent> DamageWidgetComponent = nullptr;
	
	// ----------------------------
	// --        Materials       --
	// ----------------------------

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInterface>> AllMaterials = {nullptr};

	UPROPERTY()
	TObjectPtr<UMaterialInterface> MaterialForGhosts = nullptr;
	
	// ----------------------------
	// --        Niagaras        --
	// ----------------------------
	
	UPROPERTY(Replicated)
	TObjectPtr<UNiagaraSystem> BuffTankSys_NS = nullptr;
	
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> BuffTankComp_NS = nullptr;
	
	// ----------------------------
	// --       References       --
	// ----------------------------
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly ,Category = "Unit")
	TObjectPtr<AGrid> Grid = nullptr;
	
	UPROPERTY()
	TObjectPtr<AUnit> UnitToAttackRef = nullptr;
	
	UPROPERTY()
	TObjectPtr<ABuilding> BuildingToAttackRef = nullptr;
	
	UPROPERTY()
	TObjectPtr<ABase> EnemyBase = nullptr;

	// ----------------------------
	// --          Enums         --
	// ----------------------------

	UPROPERTY(BlueprintReadOnly, Blueprintable)
	EUnitName UnitName = EUnitName::None;
	
	// ----------------------------
	// --      Timer Handles     --
	// ----------------------------

	FTimerHandle MoveTimerHandle;
	FTimerDelegate MoveTimerDelegate = nullptr;

	FTimerHandle UnitAttackAnimTimer;
	FTimerDelegate UnitAttackAnimDelegate = nullptr;

	FTimerHandle InitializationTimer;
	
	// ----------------------------
	// --        Booleans        --
	// ----------------------------

	UPROPERTY(Blueprintable, BlueprintReadOnly, Replicated)
	bool bFirstActionIsMove = false;

	UPROPERTY(Blueprintable, BlueprintReadOnly, Replicated)
	bool bHasActed = false;
	
	UPROPERTY(Blueprintable, BlueprintReadOnly, Replicated)
	bool bHasClimbed = false;

	UPROPERTY(Blueprintable, BlueprintReadOnly,Replicated)
	bool bHasMoved = false;

	UPROPERTY()
	bool bIsMoving = false;

	UPROPERTY(Replicated)
	bool bJustBecameGarrison = false;
	
	// ----------------------------
	// --          Ints          --
	// ----------------------------
	
	UPROPERTY(Replicated)
	int PathToCrossPosition = 0;

	UPROPERTY(Replicated)
	int MoveSequencePos = 0;

	// Theoretical Damage taken this turn
	UPROPERTY(BlueprintReadWrite, Blueprintable, Replicated)
	TArray<int> AllDamageInflicted = {0};
	
	// ----------------------------
	// -- Movements - FIntPoints --
	// ----------------------------

	UPROPERTY()
	FIntPoint FutureMovementPos = FIntPoint(-999, -999);
	
	UPROPERTY(Replicated)
	TArray<FIntPoint> FutureMovement = {0};
	
	UPROPERTY(Replicated)
	TArray<FIntPoint> PathToCross = {0};

	// ----------------------------
	// --   Movements - General  --
	// ----------------------------

	FVector UnitNewLocation = FVector(0,0,0);
	
	// ----------------------------
	// --        Textures        --
	// ----------------------------

	UPROPERTY()
	TObjectPtr<UTexture2D> HeavenIcon = nullptr;
	UPROPERTY()
	TObjectPtr<UTexture2D> HeavenIconDead = nullptr;
	
	UPROPERTY()
	TObjectPtr<UTexture2D> HellIcon= nullptr;
	UPROPERTY()
	TObjectPtr<UTexture2D> HellIconDead = nullptr;
	
	UPROPERTY()
	TObjectPtr<UTexture2D> NeutralIcon = nullptr;
	UPROPERTY()
	TObjectPtr<UTexture2D> NeutralIconDead = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Texture")
	TObjectPtr<UTexture2D> UnitIcon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Texture")
	TObjectPtr<UTexture2D> UnitIconDead = nullptr;

	FTimerHandle IconTimerHandle;
	
	// ----------------------------
	// --         Death          --
	// ----------------------------

	FTimerHandle DeathTimerHandle;

	UPROPERTY(Replicated)
	FRotator UnitRotation = FRotator(0,0,0);

	// ----------------------------
protected:
	// ----------------------------
	// --         Meshes         --
	// ----------------------------

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Unit", Replicated, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> UnitMesh = nullptr;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Unit", Replicated, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> UnitMeshDistinction = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
	TObjectPtr<UStaticMeshComponent> GhostsMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
	TObjectPtr<UStaticMeshComponent> GhostsFinaleLocationMesh = nullptr;
	
	// ----------------------------
	// --       References       --
	// ----------------------------
	
	UPROPERTY(Replicated)
	TObjectPtr<ACustomPlayerController> PlayerControllerRef = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
	TObjectPtr<ABuilding> BuildingRef = nullptr;
	
	// ----------------------------
	// --         Enums          --
	// ----------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "Unit")
	EPlayer PlayerOwner = EPlayer::P_Neutral;
	
	// ----------------------------
	// --        Booleans        --
	// ----------------------------

	UPROPERTY()
	bool bBeganAttack = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Unit")
	bool bBuffTank = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Unit")
	bool bIsClimbing = false;

	UPROPERTY(Replicated, VisibleAnywhere)
	bool bIsCommandeerBuffed = false;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
	bool bIsGarrison = false;

	UPROPERTY(Replicated)
	bool bIsGhosts = false;

	UPROPERTY()
	bool bIsSelected = false;

	UPROPERTY()
	bool bWillMove = true;
	
	// ----------------------------
	// --       Unit Stats       --
	// ----------------------------
	
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
	FVector UnitLocationInWorld = FVector(0,0,0);
	
	// ----------------------------
	// --    Position in Grid    --
	// ----------------------------

	UPROPERTY()
	FIntPoint IndexPosition = FIntPoint(-999, -999);

	UPROPERTY()
	TArray<FIntPoint> Path = {0};
	
	// ----------------------------
	// --         Ghosts         --
	// ----------------------------
	
	UPROPERTY()
	float DeltaTimeGhosts = 0.0f;

	UPROPERTY()
	int CurrentIndexGhost = 0;

	// UFUNCTIONS
public:
	// ----------------------------
	// --       Constructor      --
	// ----------------------------
	
	AUnit();
	
	// ----------------------------
	// --        Overrides       --
	// ----------------------------
	
	UFUNCTION()
	void Destroyed() override;
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual bool Interact_Implementation(ACustomPlayerController* PlayerController) override;

	UFUNCTION()
	virtual void DisplayWidgetTutorial();
	
	// ----------------------------
	// --      Interactions      --
	// ----------------------------
	
	UFUNCTION()
	void interaction(ACustomPlayerController *PlayerController);
	
	// ----------------------------
	// --       Turn Switch      --
	// ----------------------------
	
	UFUNCTION()
	void NewTurn();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_NewTurn();
	
	// ----------------------------
	// --          Grid          --
	// ----------------------------
	
	UFUNCTION(Server,Reliable)
	void Server_AddOnGrid();

	UFUNCTION(BlueprintCallable)
	void SetGrid(AGrid* NewGrid);
	
	// ----------------------------
	// --      Pre-Movement      --
	// ----------------------------

	UFUNCTION(NetMulticast, Reliable)
	void Multi_PrepareMove(const TArray<FIntPoint>& NewPos);
	
	// ----------------------------
	// --        Movement        --
	// ----------------------------

	UFUNCTION()
	void InitializeFullMove(TArray <FIntPoint> FullMove);
	
	UFUNCTION(BlueprintCallable,NetMulticast,Reliable)
	void UnitMoveAnim();
	
	UFUNCTION()
	virtual void MoveUnitEndTurn();

	UFUNCTION()
	void MoveCamera();

	UFUNCTION(NetMulticast,Reliable)
	void Multi_MoveCamera(ACameraPlayer* CameraPlayer);
	
	// ----------------------------
	// --         Ghosts         --
	// ----------------------------

	UFUNCTION(NetMulticast,Reliable)
	void Multi_HiddeGhosts();
	
	UFUNCTION()
	UStaticMeshComponent* GetFinalGhostMesh();
	
	// ----------------------------
	// --         Attack         --
	// ----------------------------

	UFUNCTION()
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION(BlueprintCallable)
	virtual void AttackUnit(AUnit* UnitToAttack, bool bInitiateAttack);

	UFUNCTION(BlueprintCallable,Server,Reliable)
	virtual void AttackBase(ABase* BaseToAttack);

	UFUNCTION(BlueprintCallable,Server,Reliable)
	virtual void AttackBuilding(ABuilding* BuildingToAttack);
	
	UFUNCTION()
	void AnimAttack(AActor* ThingToAttack);
	
	// ----------------------------
	// --          Buffs         --
	// ----------------------------

	UFUNCTION(Server, Reliable)
	void Server_GetBuffs();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_GetBuffs();

	UFUNCTION(Server, Reliable)
	void Server_GetBuffsVisual();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_GetBuffsVisual();
	
	// ----------------------------
	// --     Cancel Action      --
	// ----------------------------

	UFUNCTION(NetMulticast, Reliable)
	void Multi_CancelMove();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_CancelAttack();
	
	// ----------------------------
	// --        Textures        --
	// ----------------------------
	
	UFUNCTION()
	void SetUnitIcon();
	
	// ----------------------------
	// --          Death         --
	// ----------------------------

	UFUNCTION()
	void Server_DeathAnim();

	UFUNCTION()
	void Multi_DeathAnim();

	UFUNCTION(Server,Reliable)
	void Server_DestroyUnit();
	
	// ----------------------------
	// --     Leave Garrison     --
	// ----------------------------

	UFUNCTION(NetMulticast, Reliable)
	void Multi_RemoveGarrison();
	
	// ---------------------------- //
	// ---        GETTERS       --- //
	// ---------------------------- //

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

	UFUNCTION()
	bool GetWillMove();

	// Enums
	UFUNCTION()
	EPlayer GetPlayerOwner();

	// String
	UFUNCTION(BlueprintCallable,BlueprintPure)
	FString GetNameUnit();

	// FIntPoint
	UFUNCTION()
	FIntPoint GetIndexPosition();
	
	// ---------------------------- //
	// ---        SETTERS       --- //
	// ---------------------------- //

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

	UFUNCTION()
	void SetWillMove(bool bWM);

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
protected:
	// ----------------------------
	// --        Override        --
	// ----------------------------
	
	virtual void BeginPlay() override;
	
	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;
	
	// ----------------------------
	// --     Initialisation     --
	// ----------------------------

	UFUNCTION(Server, Reliable)
	void AssignPlayerController();
	
	// ----------------------------
	// --        Movement        --
	// ----------------------------

	UFUNCTION(Client,Reliable)
	void InitGhosts();

	UFUNCTION()
	void MoveGhosts(float DeltaTime);

	UFUNCTION()
	void MoveGhostsAction(float DeltaTime,const TArray<FIntPoint> &PathToFollowGhost);
	
	// ----------------------------
	// --      Initial Tests     --
	// ----------------------------

	void SetGrid();
};
