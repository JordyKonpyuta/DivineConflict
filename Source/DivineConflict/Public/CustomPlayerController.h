// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base.h"
#include "Building.h"
#include "CustomPlayerState.h"
#include "EnumsList.h"
#include "Grid.h"
#include "GameFramework/PlayerController.h"
#include "InteractInterface.h"
#include "Unit.h"
#include "CustomPlayerController.generated.h"

struct FInputActionValue;
class AGhostUnitSpawning;
class ABuilding;
class ACameraPlayer;
class AGrid;
class ACustomGameState;
class UInputMappingContext;
class UInputAction;
class AEnumsList;
class AUnit;
class ABase;
class ATower;

/**
 * 
 */
UENUM(BlueprintType)
enum class EDC_ActionPlayer : uint8
{
	None UMETA(DisplayName = "None"),
	MoveUnit UMETA(DisplayName = "MoveUnit"),
	AttackUnit UMETA(DisplayName = "AttackUnit"),
	Special UMETA(DisplayName = "Special"),
	AttackBuilding UMETA(DisplayName = "AttackBuilding"),
	SelectBuilding UMETA(DisplayNmae = "SelectBuilding"),
};

UENUM(Blueprintable)
enum class EDC_ActionType : uint8
{
	None				UMETA(DisplayName = "None"),
	UnitMove			UMETA(DisplayName = "Unit Move"),
	UnitAttack			UMETA(DisplayName = "Unit Attack"),
	TowerAttack			UMETA(DisplayName = "Tower Attack"),
	DivineAssistance	UMETA(DisplayName = "Divine Assistance"),
	SpawnUnit			UMETA(DisplayName = "Spawn Unit")
};

UENUM(Blueprintable)
enum class EDC_TargetType : uint8
{
	None		UMETA(DisplayName = "None"),
	Unit		UMETA(DisplayName = "Unit Move"),
	Base		UMETA(DisplayName = "Unit Attack"),
	Building	UMETA(DisplayName = "Tower Attack")
};

USTRUCT(Blueprintable)
struct FStructActiveActions
{
	GENERATED_BODY()

	// Actor that DOES the action
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> ActorRef;

	// Action that IS DONE; depends on the ActorRef
	UPROPERTY(BlueprintReadWrite)
	EDC_ActionType UnitAction;

	// WHAT the target (if there is one) ended up being ; use "None" if there are no targets (like if you were moving, for example)
	UPROPERTY(BlueprintReadWrite)
	EDC_TargetType TargetType;

	// WHERE the action that happens will happen
	UPROPERTY(BlueprintReadWrite)
	FIntPoint TargetLocation;
};

USTRUCT(Blueprintable)
struct FStructPassive
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> ActorRef;

	UPROPERTY(BlueprintReadWrite)
	EUnitType UnitType = EUnitType::U_Leader;

	UPROPERTY(BlueprintReadWrite)
	FIntPoint TilePosition;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AGhostUnitSpawning> GhostRef;
};

DECLARE_MULTICAST_DELEGATE(FOnTurnChangedSignature);

UCLASS()
class DIVINECONFLICT_API ACustomPlayerController : public APlayerController , public IInteractInterface
{
	GENERATED_BODY()
	
	// UPROPERTIES
public:
	// ----------------------------
	// --       References       --
	// ----------------------------
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AGrid> Grid = nullptr;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivate = "true"),Replicated)
	TObjectPtr<ACameraPlayer> CameraPlayerRef = nullptr;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Ref", meta = (AllowPrivate = "true"))
	TObjectPtr<ACustomPlayerState> PlayerStateRef = nullptr;
	
	// ----------------------------
	// --      Input Actions     --
	// ----------------------------

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	TObjectPtr<UInputAction> AIEndTurn = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	TObjectPtr<UInputAction> AIMove = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	TObjectPtr<UInputAction> AIZoom = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	TObjectPtr<UInputAction> AIRotate = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	TObjectPtr<UInputAction> AIFreeCam = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	TObjectPtr<UInputAction> AIInteraction = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	TObjectPtr<UInputAction> AICancel = nullptr;
	
	// ----------------------------
	// --      Input Context     --
	// ----------------------------
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	TObjectPtr<UInputMappingContext> InputMappingContext = nullptr;
	
	// ----------------------------
	// --      Unit Spawning     --
	// ----------------------------

	UPROPERTY()
	TArray<FIntPoint> AllCurrentSpawnPoints = {FIntPoint(-999,-999)};
	
	// ----------------------------
	// --         Actions        --
	// ----------------------------
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Ref", meta = (AllowPrivate = "true"))
	int CurrentPA = 10;
	
	// ----------------------------
	// --   Actions At Turn End  --
	// ----------------------------

	UPROPERTY(Replicated, BlueprintReadWrite)
	TArray<FStructActiveActions> AllPlayerActions;

	UPROPERTY(Replicated, BlueprintReadWrite)
	TArray<FStructPassive> AllPlayerPassive;

	// Possible Things to Interact with
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Ref", meta = (AllowPrivate = "true"))
	TObjectPtr<AUnit> UnitRef = nullptr;
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Ref", meta = (AllowPrivate = "true"))
	TObjectPtr<AUnit> UnitHovering = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ref", meta = (AllowPrivate = "true"))
	TObjectPtr<AUnit> Opponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Replicated , Category = "Ref", meta = (AllowPrivate = "true"))
	TObjectPtr<ABase> BaseRef = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ref", meta = (AllowPrivate = "true"))
	TObjectPtr<ATower> TowerRef = nullptr;
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Replicated, Category = "Input", meta = (AllowPrivate = "true"))
	TObjectPtr<ABuilding> BuildingRef = nullptr;
	
	// ----------------------------
	// --        Delegates       --
	// ----------------------------
	
	FOnTurnChangedSignature OnTurnChangedDelegate;
	
	// ----------------------------
	// --         Timers         --
	// ----------------------------

	UPROPERTY()
	FTimerHandle TimerActiveEndTurn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int InactivityTimer = 0;

	UPROPERTY()
	FTimerHandle InactivityTimerHandle;
	
	// ----------------------------
	// --           UI           --
	// ----------------------------

	UPROPERTY(Blueprintable, EditAnywhere, BlueprintReadWrite)
	bool bHoveringOverUnit = false;

	UPROPERTY(Blueprintable, EditAnywhere, BlueprintReadWrite)
	bool bHoveringOverBase = false;
	
protected:
	// ----------------------------
	// --          Teams         --
	// ----------------------------
	
	UPROPERTY(ReplicatedUsing = OnRep_PlayerTeam)
	EPlayer PlayerTeam = EPlayer::P_Neutral;
	
	// ----------------------------
	// --          Turns         --
	// ----------------------------

	UPROPERTY()
	bool bIsInActiveTurn = false;

	UPROPERTY()
	bool bIsReady = false;
	
	// ----------------------------
	// --         Actions        --
	// ----------------------------

	UPROPERTY()
	EDC_ActionPlayer PlayerAction = EDC_ActionPlayer::None;
	
	// ----------------------------
	// --     Path Reachable     --
	// ----------------------------

	UPROPERTY()
	TArray<FIntPoint> PathReachable;
	
	// ----------------------------
	// --           UI           --
	// ----------------------------

	UPROPERTY()
	bool bIsUIVisible = false;
	
	// UFUNCTIONS
public:
	// ----------------------------
	// --        Override        --
	// ----------------------------
	
	UFUNCTION()
	virtual void Tick(float DeltaTime) override;
	
	// ----------------------------
	// --         On_Reps        --
	// ----------------------------
	
	UFUNCTION()
	void OnRep_PlayerTeam();
	
	// ----------------------------
	// --      Interactions      --
	// ----------------------------

	UFUNCTION()
	void ControllerInteraction();

	UFUNCTION(BlueprintCallable)
	void VerifyBuildInteraction();
	
	// ----------------------------
	// --     Mode Selection     --
	// ----------------------------
	
	UFUNCTION(BlueprintCallable)
	void SelectModeMovement();

	UFUNCTION(BlueprintCallable)
	void SelectModeAttack();

	UFUNCTION( BlueprintCallable)
	void SelectModeAttackBuilding();

	UFUNCTION(BlueprintCallable)
	void SelectModeSpecial();
	
	UFUNCTION( BlueprintCallable)
	void SelectModeSelectBuilding();

	UFUNCTION(BlueprintCallable)
	void UpgradeBase(ABase* BaseToUpgrade);

	UFUNCTION(Server, Reliable)
	void Server_UpgradeBase();
	
	// ----------------------------
	// --     Turns (Set-Up)     --
	// ----------------------------
	
	UFUNCTION()
	void AssignPlayerPosition();

	UFUNCTION(Server, Reliable)
	void Server_SwitchPlayerTurn(const ACustomGameState *GameStateRef);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SwitchPlayerTurn(const ACustomGameState *GameStateRef);

	UFUNCTION(Server, Reliable)
	void Server_Delegate();

	UFUNCTION()
	void SetPositionInBase();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetPositionInBase(ABase* CurrentBase);

	UFUNCTION()
	void SetPositionOnUnit();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetPositionOnUnit(AUnit* CurrentUnit);
	
	// ----------------------------
	// --        End Turn        --
	// ----------------------------
	
	UFUNCTION(BlueprintCallable)
	void EndTurn();

	UFUNCTION(Server, Reliable)
	void Server_EndTurn();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FeedbackEndTurn(bool visibility);

	UFUNCTION(Server, Reliable)
	void Server_ClearMode();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_ClearMode();
	
	// ----------------------------
	// --    Actions (Active)    --
	// ----------------------------

	UFUNCTION(Server, Reliable)
	void AttackBase(ABase* BaseToAttack,AUnit* UnitAttacking);

	UFUNCTION(Server, Reliable)
	void ServerAttackUnit(AUnit* UnitToAttack, AUnit* UnitAttacking);

	UFUNCTION(Server, Reliable)
	void ServerAttackBuilding(ABuilding* BuildingToAttack, AUnit* UnitAttacking);

	UFUNCTION(Server, Reliable)
	void ServerAttackTower(ATower* TowerToAttack, AUnit* UnitAttacking);

	UFUNCTION(Server, Reliable)
	void Server_SpecialUnit(AUnit* UnitSpecial, AActor* ThingToAttack);
	
	// ----------------------------
	// --    Actions (Passive)   --
	// ----------------------------

	UFUNCTION(NotBlueprintable)
	TArray<FIntPoint> PrepareSpawnArea(TArray<FIntPoint> AllSpawnArea, FIntPoint BaseTile);
	
	UFUNCTION(Blueprintable)
	bool SpawnUnit(EUnitType UnitToSpawn, FIntPoint SpawnChosen, ABase* BaseToSpawn, ABuilding* BuildingToSpawn);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_InitServerSpawn(EUnitType UnitToSpawn, FIntPoint SpawnChosen, ABase* BaseToSpawn, ABuilding* BuildingToSpawn,ACustomPlayerState* PlayerStat);
	
	UFUNCTION(Server, Reliable)
	void Server_SpawnUnit(EUnitType UnitToSpawn, FIntPoint SpawnChosen, ABase* BaseToSpawn, ABuilding* BuildingToSpawn,ACustomPlayerState* PlayerStat);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnUnit(AUnit* UnitSpawned,AGrid* GridSpawned, ACustomPlayerState* PlayerStatRef, ABase* BaseSpawned, ABuilding* BuildingSpawned);
	
	UFUNCTION(Server, Reliable)
	void Server_SpawnBaseUnit(EUnitType UnitToSpawn,AGrid* GridSer, ABase* BaseToSpawn, EPlayer PlayerOwner);
	
	UFUNCTION(BlueprintCallable)
	void SpawnBaseUnitGhost(EUnitType UnitToSpawn);

	UFUNCTION()
	void SpawnGhostUnit(EUnitType UnitToSpawn,FIntPoint SpawnChosen);

	UFUNCTION(Server, Reliable)
	void Server_RessourceChange(const ACustomPlayerState* PSR,EUnitType UnitType);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_RessourceChange(const ACustomPlayerState* PSR,EUnitType UnitType);
	
	// ----------------------------
	// --   Actions (End Turn)   --
	// ----------------------------

	UFUNCTION(Server, Reliable)
	void Server_ActionActiveTurn();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_ActionActiveTurn();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_ActionActiveTurnRedo();

	UFUNCTION(Server, Reliable)
	void Server_CheckPlayerActionPassive();

	UFUNCTION(Server, Reliable)
	void Server_ActionPassiveTurn();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_ActionPassiveTurn();
	
	// ----------------------------
	// --     Cancel Actions     --
	// ----------------------------
	
	UFUNCTION(Server, Reliable)
	void Server_CancelLastAction();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_CancelLastAction();
	
	// ----------------------------
	// --     "PathFinding"      --
	// ----------------------------

	UFUNCTION(Server, Reliable)
	void ServerMoveUnit(const AUnit* UnitToMove);

	UFUNCTION(Server, Reliable)
	void Server_PrepareMoveUnit(const TArray<FIntPoint> &Path, const AUnit* UnitToMove);
	
	// ----------------------------
	// --           UI           --
	// ----------------------------

		// General
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateUi();
	
	UFUNCTION(Client, Reliable)
	void Client_UpdateUi();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateWidget3D(int Index, bool bVisibility);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ActionMemory(EDC_ActionType Action);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RemoveLastActionMemory();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FailedTutorial();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DisplayInputsOnHUD();

	UFUNCTION()
	void IncrementInactivityTimer();

	// Stats
	UFUNCTION(BlueprintNativeEvent)
	void DisplayWidget();
	
	UFUNCTION(BlueprintNativeEvent)
	void DisplayWidgetHovering();
	
	UFUNCTION(BlueprintNativeEvent)
	void DisplayWidgetBuilding();
	
	UFUNCTION(BlueprintNativeEvent)
	void DisplayWidgetBase();
	
	UFUNCTION(BlueprintNativeEvent)
	void DisplayWidgetTower();

		// Attack
	UFUNCTION(BlueprintNativeEvent)
	void DisplayAttackWidget();
	UFUNCTION(BlueprintNativeEvent)
	void RemoveAttackWidget();

		// Timers
	UFUNCTION(BlueprintNativeEvent)
	void UpdateUITimer(int TimeLeft);
	UFUNCTION(Client, Reliable)
	void Client_PauseTimerUI();
	UFUNCTION(BlueprintNativeEvent)
	void PauseTimerUI();

		// Hide
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HideWidget();
	
	// ---------------------------- //
	// --         GETTERS        -- //
	// ---------------------------- //

		// Turns
	UFUNCTION(BlueprintCallable)
	bool GetIsInActiveTurn();

	UFUNCTION(BlueprintCallable)
	bool GetIsReady();
	
		// Player Team

	UFUNCTION(BlueprintCallable)
	EPlayer GetPlayerTeam();

		// Player Actions
	UFUNCTION()
	EDC_ActionPlayer GetPlayerAction();

		// "PathFinding"
	UFUNCTION()
	TArray<FIntPoint> GetPathReachable();
	
	// ---------------------------- //
	// --         SETTERS        -- //
	// ---------------------------- //

		// Turns

	UFUNCTION(BlueprintCallable)
	void SetIsInActiveTurn(bool bA);

	UFUNCTION(BlueprintCallable)
	void SetIsReady(bool bR);

		// Player Team

	UFUNCTION(BlueprintCallable)
	void SetPlayerTeam(EPlayer PT);

		// Player Actions

	UFUNCTION()
	void SetPlayerAction(EDC_ActionPlayer PA);

		// "PathFinding"

	UFUNCTION()
	void AddToPathReachable(FIntPoint NewPoint);

	UFUNCTION()
	void SetPathReachable(TArray<FIntPoint> NewPath);
	
	
protected:
	// ----------------------------
	// --        Overrides       --
	// ----------------------------
	
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;
	
	// ----------------------------
	// --          Grid          --
	// ----------------------------
	
	void SetGrid();
	
	// ----------------------------
	// --      "PathFinding"     --
	// ----------------------------
	
	UFUNCTION()
	void FindReachableTiles();
	
	// ----------------------------
	// --     Action Players     --
	// ----------------------------

	UFUNCTION()
	void RotateCamera(const FInputActionValue& Value);

	UFUNCTION()
	void RotateCameraPitch(const FInputActionValue& Value);

	UFUNCTION()
	void ZoomCamera( const FInputActionValue& Value);

	UFUNCTION()
	void CancelAction(const FInputActionValue& Value);
};
