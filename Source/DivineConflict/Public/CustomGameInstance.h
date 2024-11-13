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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomGameInstance")
	UDC_SaveGameSystem* SaveGameInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomGameInstance")
	ACameraPlayer* CameraPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomGameInstance")
	ACustomPlayerState* CustomPlayerState;
	

private:

	
/* UFUNCTIONS */	
public:

	UCustomGameInstance();

	UFUNCTION()
	virtual void Init() override;
	

	UFUNCTION(BlueprintCallable, Category = "CustomGameInstance")
	void SaveGame();

	UFUNCTION(BlueprintCallable, Category = "CustomGameInstance")
	void EnableFaceBottomKey(bool enable);
	
private:
};
