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
	// ----------------------------
	// References
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "PlayerControllers", meta = (AllowPrivateAccess = "true"))
	TArray<ACustomPlayerController*> PlayerControllers;

	UPROPERTY(BlueprintReadOnly, Category = "Refs")
	TArray<ACustomPlayerState*> AllPlayerStates;

	// WIN/LOSE CON
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Game")
	ACustomPlayerState* PWinner = nullptr;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Game")
	ACustomPlayerState* PLoser = nullptr;

	// ----------------------------
	// Loading Timer
	
	FTimerHandle BeginningTimerHandle;
	
	UPROPERTY(EditAnywhere, NotBlueprintable, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	int LoadingTimer = 2;
	
	// ----------------------------
	// Turns
	
	FTimerHandle TurnTimerHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	int TurnTimerLength = 90;

	UPROPERTY(BlueprintReadOnly, Category = "Turns")
	bool bP1Turn = false;

	UPROPERTY(Blueprintable,Replicated, BlueprintReadWrite, Category = "Turns")
	int Turn = 0;
	
	// ----------------------------
	// Delegates
	
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnTurnSwitchDelegate OnTurnSwitchDelegate;
	
private:
	
/* UFUNCTIONS */	
public:
	// ----------------------------
	// Turns
	
	UFUNCTION(NotBlueprintable, Category = "TurnSystem")
	void AssignTurnOrder();
	
	UFUNCTION(BlueprintCallable)
	void AssignPlayerTurns();
	
	UFUNCTION(BlueprintCallable, Category = "TurnSystem")
	void BeginTimer();

	UFUNCTION(BlueprintCallable, Category = "TurnSystem")
	void SwitchPlayerTurn();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSwitchPlayerTurn();

	UFUNCTION()
	void CheckSwitchPlayerTurn();

	// ----------------------------
	// Widgets
	
	UFUNCTION(BlueprintNativeEvent)
	void DisplayWidget();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDisplayWidget();
	
	UFUNCTION(Server, Reliable)
	void ServerDisplayWidget();
	
	// ----------------------------
	// End Game
	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerVictoryScreen(EPlayer Loser);
	
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void MulticastVictoryScreen(EPlayer Loser);
	
private:
	// ----------------------------
	// Overrides

	virtual void BeginPlay() override;
};
