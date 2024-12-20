// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "CustomGameState.generated.h"


class ASequenceTuto;
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
	// --       References       --
	// ----------------------------
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "PlayerControllers", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<ACustomPlayerController>> PlayerControllers = {nullptr};

	UPROPERTY(BlueprintReadOnly, Category = "Refs")
	TArray<TObjectPtr<ACustomPlayerState>> AllPlayerStates = {nullptr};

	UPROPERTY()
	TObjectPtr<ASequenceTuto> SequenceTutoRef = nullptr;
	
	// ----------------------------
	// --   WIN/LOSE Condition   --
	// ----------------------------
	
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Game")
	TObjectPtr<ACustomPlayerState> PWinner = nullptr;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Game")
	TObjectPtr<ACustomPlayerState> PLoser = nullptr;
	
	// ----------------------------
	// --     Timer - Loading    --
	// ----------------------------
	
	FTimerHandle BeginningTimerHandle;
	
	UPROPERTY(EditAnywhere, NotBlueprintable, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	int LoadingTimer = 2;
	
	// ----------------------------
	// --      Timer - Turns     --
	// ----------------------------
	
	FTimerHandle TurnTimerHandle;

	FTimerHandle SwitchPlayerTurnHandle;
	
	// ----------------------------
	// --         Turns          --
	// ----------------------------

	UPROPERTY(Blueprintable,Replicated, BlueprintReadWrite, Category = "Turns")
	int Turn = 0;

	UPROPERTY(BlueprintReadOnly,Replicated, Category = "Turns")
	bool bBlockTurnSwitch = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	int TurnTimerLength = 60;
	
	UPROPERTY()
	int BlockTurnSwitchTimerLength = 1;
	
	// ----------------------------
	// --        Delegates       --
	// ----------------------------
	
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnTurnSwitchDelegate OnTurnSwitchDelegate;
	
private:
	
/* UFUNCTIONS */	
public:
	// ----------------------------
	// --          Turns         --
	// ----------------------------
	
	UFUNCTION(NotBlueprintable, Category = "TurnSystem")
	void AssignTurnOrder();
	
	UFUNCTION(BlueprintCallable)
	void AssignPlayerTurns();
	
	UFUNCTION(BlueprintCallable, Category = "TurnSystem")
	void BeginTimer();
	
	UFUNCTION()
	void SwitchIsBlockTurnSwitchTimer();
	
	UFUNCTION(BlueprintCallable, Category = "TurnSystem")
	void SwitchPlayerTurn();

	UFUNCTION(Server, Reliable)
	void Server_SwitchPlayerTurn();

	UFUNCTION()
	void CheckSwitchPlayerTurn();

	// ----------------------------
	// --   Actions (End Turn)   --
	// ----------------------------

	UFUNCTION()
	void CheckPlayerActionActive();

	UFUNCTION()
	void CheckPlayerActionPassive();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SendPlayerAction(ACustomPlayerState* PlayerState);

	UFUNCTION()
	void PauseTimerUI();

	// ----------------------------
	// --         Widgets        --
	// ----------------------------
	
	UFUNCTION(BlueprintNativeEvent)
	void DisplayWidget();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDisplayWidget();
	
	UFUNCTION(Server, Reliable)
	void ServerDisplayWidget();

	UFUNCTION(BlueprintNativeEvent)
	void DisplayEndTutorialWidget();
	
	// ----------------------------
	// --        End Game        --
	// ----------------------------
	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerVictoryScreen(EPlayer Loser);
	
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void MulticastVictoryScreen(EPlayer Loser);
	
private:
	// ----------------------------
	// --        Overrides       --
	// ----------------------------

	virtual void BeginPlay() override;
};
