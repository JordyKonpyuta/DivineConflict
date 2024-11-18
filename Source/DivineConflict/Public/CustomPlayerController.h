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

USTRUCT(Blueprintable)
struct FStructActions
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> ActorRef;

	UPROPERTY(BlueprintReadWrite)
	EDC_ActionPlayer UnitAction;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AUnit> UnitAttacking;
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
	// References
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AGrid> Grid;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivate = "true"))
	TObjectPtr<ACameraPlayer> CameraPlayerRef;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Ref", meta = (AllowPrivate = "true"))
	TObjectPtr<ACustomPlayerState> PlayerStateRef;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	TObjectPtr<UInputAction> AIEndTurn;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	TObjectPtr<UInputAction> AIMove;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	TObjectPtr<UInputAction> AIZoom;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	TObjectPtr<UInputAction> AIRotate;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	TObjectPtr<UInputAction> AIFreeCam;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	TObjectPtr<UInputAction> AIInteraction;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	TObjectPtr<UInputAction> AIRemovePath;
	
	// ----------------------------
	// Inputs Context
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	TObjectPtr<UInputMappingContext> InputMappingContext;
	
	
	// ----------------------------
	// Unit Spawning

	UPROPERTY()
	TArray<FIntPoint> AllCurrentSpawnPoints;
	
	// ----------------------------
	// Actions 
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Ref", meta = (AllowPrivate = "true"))
	int CurrentPA = 10;
	
	// ----------------------------
	// Actions at Turn End

	UPROPERTY(Replicated, BlueprintReadWrite)
	TArray<FStructActions> AllPlayerActions;

	UPROPERTY(Replicated, BlueprintReadWrite)
	TArray<FStructPassive> AllPlayerPassive;

		// Possible Things to Interact with
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Ref", meta = (AllowPrivate = "true"))
	TObjectPtr<AUnit> UnitRef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ref", meta = (AllowPrivate = "true"))
	TObjectPtr<AUnit> Opponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Replicated , Category = "Ref", meta = (AllowPrivate = "true"))
	TObjectPtr<ABase> BaseRef;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ref", meta = (AllowPrivate = "true"))
	TObjectPtr<ATower> TowerRef;
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Replicated, Category = "Input", meta = (AllowPrivate = "true"))
	TObjectPtr<ABuilding> BuildingRef;
	
	// ----------------------------
	// Delegates
	
	FOnTurnChangedSignature OnTurnChangedDelegate;
	
	// ----------------------------
	// Timers

	UPROPERTY()
	FTimerHandle TimerActiveEndTurn;

	
protected:
	
	// ----------------------------
	// Teams
	
	UPROPERTY(ReplicatedUsing = OnRep_PlayerTeam)
	EPlayer PlayerTeam = EPlayer::P_Neutral;
	
	// ----------------------------
	// Turns

	UPROPERTY()
	bool IsInActiveTurn = false;

	UPROPERTY()
	bool IsReady = false;
	
	// ----------------------------
	// Actions

	UPROPERTY()
	EDC_ActionPlayer PlayerAction = EDC_ActionPlayer::None;

	UPROPERTY()
	bool IsSelectingUnitFromBuilding;
	
	// ----------------------------
	// Pathfinding

	UPROPERTY()
	TArray<FIntPoint> PathReachable;
	
	// UFUNCTIONS
public:
	
	// ----------------------------
	// Override
	
	UFUNCTION()
	virtual void Tick(float DeltaTime) override;
	
	// ----------------------------
	// On_Reps
	UFUNCTION()
	void OnRep_PlayerTeam();
	
	// ----------------------------
	// Interactions

	UFUNCTION()
	void ControllerInteraction();

	UFUNCTION(BlueprintCallable)
	void VerifyBuildInteraction();
	
	// ----------------------------
	// Mode Selection
	
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
	
	// ----------------------------
	// Turns - Set-up
	
	UFUNCTION()
	void AssignPlayerPosition();

	UFUNCTION(Server, Reliable)
	void Server_SwitchPlayerTurn(const ACustomGameState *GameStateRef);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SwitchPlayerTurn(const ACustomGameState *GameStateRef);

	UFUNCTION(Server, Reliable)
	void Server_Delegate();
	
	// ----------------------------
	// End Turn
	
	UFUNCTION()
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
	// Actions - Active

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

	UFUNCTION(Server, Reliable)
	void Server_ActivateSpecial(AUnit* Unit, FIntPoint NewPos);
	
	// ----------------------------
	// Actions - Passive

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
	// Cancel Actions
	
	UFUNCTION(Server, Reliable)
	void Server_CancelLastAction();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_CancelLastAction();
	
	// ----------------------------
	// Actions - End Turn

	UFUNCTION(Server, Reliable)
	void Server_ActionActiveTurn();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_ActionActiveTurn();

	UFUNCTION(Server, Reliable)
	void Server_CheckPlayerActionPassive();

	UFUNCTION(Server, Reliable)
	void Server_ActionPassiveTurn();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_ActionPassiveTurn();
	
	// ----------------------------
	// "Pathfinding"

	UFUNCTION(Server, Reliable)
	void ServerMoveUnit(const AUnit* UnitToMove);

	UFUNCTION(Server, Reliable)
	void Server_PrepareMoveUnit(const TArray<FIntPoint> &Path, const AUnit* UnitToMove);
	
	// ----------------------------
	// UI

		// General
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateUi();
	UFUNCTION(Client, Reliable)
	void Client_UpdateUi();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateWidget3D(int Index, bool bVisibility);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ActionMemory(EDC_ActionPlayer Action);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RemoveLastActionMemory();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FailedTutorial();

		// Stats
	UFUNCTION(BlueprintNativeEvent)
	void DisplayWidget();
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
	void HiddeWidget();
	
	// ----------------------------
	// GETTERS

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
	
	
	// ----------------------------
	// SETTERS

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
	// Overrides
	
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;
	
	// ----------------------------
	// Grid
	
	void setGrid();
	
	// ----------------------------
	// "Pathfinding"
	
	UFUNCTION()
	void FindReachableTiles();

	// ----------------------------
	// ActionPlayer
	
	void RotateCamera(const FInputActionValue& Value);

	void RotateCameraPitch(const FInputActionValue& Value);

	void ZoomCamera( const FInputActionValue& Value);

	void PathRemove(const FInputActionValue& Value);
};
