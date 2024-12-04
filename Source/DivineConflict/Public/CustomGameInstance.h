// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CustomGameInstance.generated.h"

/**
 * 
 */

class UDC_SaveGameSystem;
class USaveGame;
class ACameraPlayer;
class ACustomPlayerState;

UCLASS()
class DIVINECONFLICT_API UCustomGameInstance : public UGameInstance
{
	GENERATED_BODY()

/* UPROPERTIES */
public:
	// ----------------------------
	// --       References       --
	// ----------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomGameInstance")
	TObjectPtr<UDC_SaveGameSystem> SaveGameInstance = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomGameInstance")
	TObjectPtr<ACameraPlayer> CameraPlayer = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomGameInstance")
	TObjectPtr<ACustomPlayerState> CustomPlayerState = nullptr;
	

private:

	
/* UFUNCTIONS */	
public:
	// ----------------------------
	// --       Constructor      --
	// ----------------------------
	
	UCustomGameInstance();
	
	// ----------------------------
	// --        Overrides       --
	// ----------------------------
	
	UFUNCTION()
	virtual void Init() override;
	
	// ----------------------------
	// --       Save States      --
	// ----------------------------
	
	UFUNCTION(BlueprintCallable, Category = "CustomGameInstance")
	void SaveGame();
	
	// ----------------------------
	// --           ???          --
	// ----------------------------
	
	UFUNCTION(BlueprintCallable, Category = "CustomGameInstance")
	void EnableFaceBottomKey(bool enable);
	
private:
};
