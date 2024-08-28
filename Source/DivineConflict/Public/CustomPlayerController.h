// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CustomPlayerController.generated.h"


class ACameraPlayer;
class AGrid;
class UInputMappingContext;
class UInputAction;
/**
 * 
 */
UCLASS()
class DIVINECONFLICT_API ACustomPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = "true"))
	AGrid* Grid;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivate = "true"))
	ACameraPlayer* CameraPlayerRef;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	UInputMappingContext* InputMappingContext;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	UInputAction* AIZoom;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	UInputAction* AIMove;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	UInputAction* AIRotate;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	UInputAction* AIFreeCam;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	UInputAction* AIInteraction;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	UInputAction* AIRemovePath;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	UInputAction* AIEndTurn;
	
	
protected:

	UPROPERTY()
	bool IsHell = false;

	UPROPERTY()
	bool IsInActiveTurn = false;

	UPROPERTY()
	bool IsReady = false;
	
	virtual void BeginPlay() override;

public:
	
	UFUNCTION(BlueprintCallable)
	bool GetIsHell();

	UFUNCTION(BlueprintCallable)
	void SetIsHell(bool h);

	UFUNCTION(BlueprintCallable)
	bool GetIsInActiveTurn();

	UFUNCTION(BlueprintCallable)
	void SetIsInActiveTurn(bool a);

	UFUNCTION(BlueprintCallable)
	bool GetIsReady();

	UFUNCTION(BlueprintCallable)
	void SetIsReady(bool r);
	
};
