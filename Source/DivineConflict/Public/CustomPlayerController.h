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
	
	
	
protected:

	UPROPERTY()
	bool IsHell = false;

	UPROPERTY()
	bool IsInActiveTurn = false;

	UPROPERTY()
	bool IsReady = false;
	
	virtual void BeginPlay() override;

	virtual void  SetupInputComponent() override;

	void setGrid();

	
	

	void Move(/*const FInputActionValue& Value*/);

public:
	
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
	
	void ControllerInteration();
	
};
