// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomPlayerState.h"
#include "EnumsList.h"
#include "GameFramework/PlayerController.h"
#include "InteractInterface.h"
#include "CustomPlayerController.generated.h"

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
	ACustomGameState* GameStateRef;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Ref", meta = (AllowPrivate = "true"))
	ACustomPlayerState* PlayerStateRef;
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Ref", meta = (AllowPrivate = "true"))
	int CurrentPA = 10;
	

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Ref", meta = (AllowPrivate = "true"))
	AUnit* UnitRef = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ref", meta = (AllowPrivate = "true"))
	ABase* BaseRef = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ref", meta = (AllowPrivate = "true"))
	ATower* TowerRef = nullptr;
	

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	ABuilding* BuildingRef = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Ref", meta = (AllowPrivate = "true"))
	TArray<AUnit*> Units;
	
	
protected:

	UPROPERTY()
	bool IsHell = false;
	
	UPROPERTY()
	EPlayer PlayerTeam = EPlayer::P_Hell;

	UPROPERTY()
	bool IsInActiveTurn = false;

	UPROPERTY()
	bool IsSelectingUnitFromBuilding;

	UPROPERTY()
	bool IsReady = false;

	UPROPERTY()
	bool IsDead = false;

	UPROPERTY()
	EDC_ActionPlayer PlayerAction = EDC_ActionPlayer::None;

	UPROPERTY()
	TArray<FIntPoint> PathReachable;
	
	virtual void BeginPlay() override;

	virtual void  SetupInputComponent() override;

	void setGrid();

	UFUNCTION()
	void FindReachableTiles();

	



public:


	UFUNCTION(BlueprintCallable)
	void SelectModeMovement();

	UFUNCTION(BlueprintCallable)
	void SelectModeAttack();

	UFUNCTION(	BlueprintCallable)
	void SelectModeSpecial();

	UFUNCTION( BlueprintCallable)
	void SelectModeBuilding();
	
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

	UFUNCTION(BlueprintCallable)
	bool GetIsDead();

	UFUNCTION(BlueprintCallable)
	void SetIsDead(bool bD);

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

	UFUNCTION(NotBlueprintable)
	TArray<FIntPoint> PrepareSpawnArea(TArray<FIntPoint> AllSpawnArea, FIntPoint BaseTile);
	
	UFUNCTION(Blueprintable)
	bool SpawnUnit(EUnitType UnitToSpawn, FIntPoint SpawnChosen);

	UFUNCTION(Server, Reliable)
	void ServerMoveUnit(const TArray<FIntPoint> &PathToMove , const AUnit* UnitToMove);

	UFUNCTION(Server, Reliable)
	void Server_SpawnBaseUnit(EUnitType UnitToSpawn,AGrid* GridSer, ABase* BaseToSpawn, EPlayer PlayerOwner);
	
	UFUNCTION(BlueprintCallable)
	void SpawnBaseUnit(EUnitType UnitToSpawn);
	
	UFUNCTION()
	void EndTurn();

	UFUNCTION(Server, Reliable)
	void Server_EndTurn();

	UFUNCTION(BlueprintNativeEvent)
	void UpdateUi();
};
