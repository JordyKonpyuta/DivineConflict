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
	SpellCast UMETA(DisplayName = "SpellCast"),
	AttackBuilding UMETA(DisplayName = "AttackBuilding"),
	SelectBuilding UMETA(DisplayNmae = "SelectBuilding"),
};

USTRUCT()
struct FStructActions
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* ActorRef;

	UPROPERTY()
	EDC_ActionPlayer UnitAction;

	UPROPERTY()
	AUnit* UnitAttacking;
};

USTRUCT()
struct FStructPassive
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* ActorRef;

	UPROPERTY()
	EUnitType UnitType = EUnitType::U_Leader;

	UPROPERTY()
	FIntPoint TilePosition;

	UPROPERTY()
	AGhostUnitSpawning* GhostRef;
};

DECLARE_MULTICAST_DELEGATE(FOnTurnChangedSignature);

UCLASS()
class DIVINECONFLICT_API ACustomPlayerController : public APlayerController , public IInteractInterface
{
	GENERATED_BODY()
public:
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = "true"))
	AGrid* Grid;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivate = "true"))
	ACameraPlayer* CameraPlayerRef;
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	UInputMappingContext* InputMappingContext;
	

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Ref", meta = (AllowPrivate = "true"))
	ACustomPlayerState* PlayerStateRef;
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Ref", meta = (AllowPrivate = "true"))
	int CurrentPA = 10;
	

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Ref", meta = (AllowPrivate = "true"))
	AUnit* UnitRef = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Replicated , Category = "Ref", meta = (AllowPrivate = "true"))
	ABase* BaseRef = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ref", meta = (AllowPrivate = "true"))
	ATower* TowerRef = nullptr;
	

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Replicated, Category = "Input", meta = (AllowPrivate = "true"))
	ABuilding* BuildingRef = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Ref", meta = (AllowPrivate = "true"))
	TArray<AUnit*> Units;
	
	TArray<FIntPoint> AllCurrentSpawnPoints;

	UPROPERTY(Replicated)
	TArray<FStructActions> AllPlayerActions;

	UPROPERTY(Replicated)
	TArray<FStructPassive> AllPlayerPassive;
/*
	UFUNCTION(Server, Reliable)
	void DoActions();*/

	FOnTurnChangedSignature OnTurnChangedDelegate;
	
protected:

	UPROPERTY()
	bool IsHell = false;
	
	UPROPERTY(ReplicatedUsing = OnRep_PlayerTeam)
	EPlayer PlayerTeam = EPlayer::P_Neutral;

	UPROPERTY()
	bool IsInActiveTurn = false;

	UPROPERTY()
	bool IsSelectingUnitFromBuilding;

	UPROPERTY()
	bool IsReady = false;

	UPROPERTY()
	EDC_ActionPlayer PlayerAction = EDC_ActionPlayer::None;

	UPROPERTY()
	TArray<FIntPoint> PathReachable;

	UPROPERTY()
	FTimerHandle TimerActiveEndTurn;
	
	virtual void BeginPlay() override;

	virtual void  SetupInputComponent() override;

	void setGrid();

	UFUNCTION()
	void FindReachableTiles();



public:

	UFUNCTION()
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnRep_PlayerTeam();
	
	UFUNCTION(BlueprintCallable)
	void SelectModeMovement();

	UFUNCTION(BlueprintCallable)
	void SelectModeAttack();

	UFUNCTION(	BlueprintCallable)
	void SelectModeSpecial();

	UFUNCTION( BlueprintCallable)
	void SelectModeAttackBuilding();
	
	UFUNCTION( BlueprintCallable)
	void SelectModeSelectBuilding();
	
	UFUNCTION(BlueprintCallable)
	bool GetIsHell();

	UFUNCTION(BlueprintCallable)
	void SetIsHell(bool bH);

	UFUNCTION(BlueprintCallable)
	EPlayer GetPlayerTeam();

	UFUNCTION(BlueprintCallable)
	void SetPlayerTeam(EPlayer PT);
	
	UFUNCTION(BlueprintCallable)
	bool GetSelectingUnitFromBuilding();

	UFUNCTION(BlueprintCallable)
	void SetSelectingUnitFromBuilding(bool bS);

	UFUNCTION(BlueprintCallable)
	bool GetIsInActiveTurn();

	UFUNCTION(BlueprintCallable)
	void SetIsInActiveTurn(bool bA);

	UFUNCTION(BlueprintCallable)
	bool GetIsReady();

	UFUNCTION(BlueprintCallable)
	void SetIsReady(bool bR);

	UFUNCTION()
	void SetPlayerAction(EDC_ActionPlayer PA);
	
	void ControllerInteraction();

	TArray<FIntPoint> GetPathReachable();
	
	UFUNCTION(BlueprintNativeEvent)
	void DisplayWidget();
	UFUNCTION(BlueprintNativeEvent)
	void DisplayWidgetBuilding();
	UFUNCTION(BlueprintNativeEvent)
	void DisplayWidgetBase();
	UFUNCTION(BlueprintNativeEvent)
	void DisplayWidgetTower();

	UFUNCTION(BlueprintNativeEvent)
	void HiddeWidget();

	UFUNCTION(NotBlueprintable)
	TArray<FIntPoint> PrepareSpawnArea(TArray<FIntPoint> AllSpawnArea, FIntPoint BaseTile);

	UFUNCTION(Server, Reliable)
	void AttackBase(ABase* BaseToAttack,AUnit* UnitAttacking);

	UFUNCTION(Server, Reliable)
	void ServerAttackUnit(AUnit* UnitToAttack, AUnit* UnitAttacking);

	UFUNCTION(Server, Reliable)
	void ServerAttackBuilding(ABuilding* BuildingToAttack, AUnit* UnitAttacking);

	UFUNCTION(Server, Reliable)
	void ServerAttackTower(ATower* TowerToAttack, AUnit* UnitAttacking);
	
	UFUNCTION(Blueprintable)
	bool SpawnUnit(EUnitType UnitToSpawn, FIntPoint SpawnChosen, ABase* BaseToSpawn, ABuilding* BuildingToSpawn);

	UFUNCTION(Server, Reliable)
	void Server_SpawnUnit(EUnitType UnitToSpawn, FIntPoint SpawnChosen, ABase* BaseToSpawn, ABuilding* BuildingToSpawn);

	UFUNCTION(Server, Reliable)
	void Multicast_SpawnUnit(AUnit* UnitSpawned,AGrid* GridSpawned, ACustomPlayerState* PlayerStatRef, ABase* BaseSpawned, ABuilding* BuildingSpawned);

	UFUNCTION(Server, Reliable)
	void ServerMoveUnit(const AUnit* UnitToMove);

	UFUNCTION(Server, Reliable)
	void Server_SpawnBaseUnit(EUnitType UnitToSpawn,AGrid* GridSer, ABase* BaseToSpawn, EPlayer PlayerOwner);
	
	UFUNCTION(BlueprintCallable)
	void SpawnBaseUnitGhost(EUnitType UnitToSpawn);

	UFUNCTION()
	void SpawnGhostUnit(EUnitType UnitToSpawn,FIntPoint SpawnChosen);

	UFUNCTION(Server, Reliable)
	void Server_PrepareMoveUnit(const TArray<FIntPoint> &Path, const AUnit* UnitToMove);
	
	UFUNCTION()
	void EndTurn();

	UFUNCTION(Server, Reliable)
	void Server_EndTurn();

	UFUNCTION()
	void ActionEndTurn();

	UFUNCTION(Server, Reliable)
	void Server_ActionActiveTurn();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_ActionActiveTurn();

	UFUNCTION(Server, Reliable)
	void Server_ActionPassiveTurn();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_ActionPassiveTurn();
	UFUNCTION(Server, Reliable)
	void Server_Delegate();

	UFUNCTION(Server, Reliable)
	void Server_SwitchPlayerTurn(const ACustomGameState *GameStateRef);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SwitchPlayerTurn(const ACustomGameState *GameStateRef);

	UFUNCTION(Server, Reliable)
	void Server_CheckPlayerActionPassive();
	
	UFUNCTION()
	void CancelLastAction();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateUi();
	UFUNCTION(Client, Reliable)
	void Client_UpdateUi();
	
	UFUNCTION(BlueprintNativeEvent)
	void UpdateUITimer(int TimeLeft);

	UFUNCTION(Client, Reliable)
	void Client_PauseTimerUI();

	UFUNCTION(BlueprintNativeEvent)
	void PauseTimerUI();
	
	UFUNCTION(BlueprintNativeEvent)
	void UpdateWidget3D(int Index, bool bVisibility);
	UFUNCTION(BlueprintNativeEvent)
	void DisplayAttackWidget();
	UFUNCTION(BlueprintNativeEvent)
	void RemoveAttackWidget();

	UFUNCTION(BlueprintCallable)
	void VerifyBuildInteraction();
	
	UFUNCTION()
	void AssignPlayerPosition();

	UFUNCTION()
	void SetPathReachable(TArray<FIntPoint> NewPath);

	UFUNCTION()
	void AddToPathReachable(FIntPoint NewPoint);
};
