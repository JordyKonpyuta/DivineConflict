// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "CustomGameState.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTurnSwitchDelegate);

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
	int TurnTimerLength = 90;

	UPROPERTY(EditAnywhere, NotBlueprintable, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	int LoadingTimer = 2;

	UPROPERTY(BlueprintReadOnly, Category = "Refs")
	TArray<ACustomPlayerState*> AllPlayerStates;

	UPROPERTY(BlueprintReadOnly, Category = "Turns")
	bool bP1Turn = false;
	
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnTurnSwitchDelegate OnTurnSwitchDelegate;

	UPROPERTY(Blueprintable,Replicated, BlueprintReadWrite, Category = "Turns")
	int Turn = 0;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Game")
	ACustomPlayerState* PWinner = nullptr;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Game")
	ACustomPlayerState* PLoser = nullptr;
	
private:
	virtual void BeginPlay() override;

	
/* UFUNCTIONS */	
public:
	UFUNCTION(BlueprintCallable)
	void AssignPlayerTurns();

	UFUNCTION(BlueprintCallable, Category = "TurnSystem")
	void SwitchPlayerTurn();

	UFUNCTION()
	void CheckSwitchPlayerTurn();
	
	FTimerHandle TurnTimerHandle;
	
	UFUNCTION(BlueprintCallable, Category = "TurnSystem")
	void BeginTimer();
	
	FTimerHandle BeginningTimerHandle;
	
	UFUNCTION(NotBlueprintable, Category = "TurnSystem")
	void AssignTurnOrder();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSwitchPlayerTurn();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerVictoryScreen(EPlayer Loser);
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void MulticastVictoryScreen(EPlayer Loser);
	UFUNCTION(BlueprintNativeEvent)
	void DisplayWidget();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDisplayWidget();
	UFUNCTION(Server, Reliable)
	void ServerDisplayWidget();
	
private:
};
