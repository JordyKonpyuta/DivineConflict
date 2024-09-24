// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InteractInterface.h"
#include "CustomPlayerController.generated.h"

class ACameraPlayer;
class AGrid;
class ACustomGameState;
class UInputMappingContext;
class UInputAction;
class AUnit;
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

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	AUnit* UnitRef = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Ref", meta = (AllowPrivate = "true"))
	TArray<AUnit*> Units;
	
	
protected:

	UPROPERTY()
	bool IsHell = false;

	UPROPERTY()
	bool IsInActiveTurn = false;

	UPROPERTY()
	bool IsReady = false;

	UPROPERTY()
	bool IsDead = false;

	UPROPERTY()
	EDC_ActionPlayer PlayerAction = EDC_ActionPlayer::None;
	
	virtual void BeginPlay() override;

	virtual void  SetupInputComponent() override;

	void setGrid();

	
	



public:


	UFUNCTION(BlueprintCallable)
	void SelectModeMovement();

	UFUNCTION(BlueprintCallable)
	void SelectModeAttack();

	UFUNCTION(	BlueprintCallable)
	void SelectModeSpell();

	UFUNCTION( BlueprintCallable)
	void SelectModeBuilding();
	
	UFUNCTION(BlueprintCallable)
	bool GetIsHell();

	UFUNCTION(BlueprintCallable)
	void SetIsHell(bool bH);

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
	
	void ControllerInteraction();

	UFUNCTION(BlueprintNativeEvent)
	void DisplayWidget();

	/*UFUNCTION(BlueprintNativeEvent)
	void DisplayWidgetEndGame();*/
	
	
};
