// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CustomGameState.generated.h"

class ACustomPlayerController;
/**
 * 
 */
UCLASS()
class DIVINECONFLICT_API ACustomGameState : public AGameStateBase
{
	GENERATED_BODY()

	
/* UPROPERTIES */
public:
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "PlayerControllers", meta = (AllowPrivateAccess = "true"))
	TArray<ACustomPlayerController*> PlayerControllers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	int TurnTimerLength = 5;

	UPROPERTY(EditAnywhere, NotBlueprintable, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	int LoadingTimer = 2;

private:
	virtual void BeginPlay() override;

	
/* UFUNCTIONS */	
public:
	UFUNCTION(BlueprintCallable)
	void AssignPlayerTurns();

	UFUNCTION(BlueprintCallable, Category = "TurnSystem")
	void SwitchPlayerTurn();

	FTimerHandle TurnTimerHandle;
	
	UFUNCTION(BlueprintCallable, Category = "TurnSystem")
	void BeginTimer();
	
	FTimerHandle BeginningTimerHandle;
	
	UFUNCTION(NotBlueprintable, Category = "TurnSystem")
	void AssignTurnOrder();
	
private:
};
