// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CustomGameMode.generated.h"

class UCustomGameInstance;

/**
 * 
 */
UCLASS()
class DIVINECONFLICT_API ACustomGameMode : public AGameModeBase
{
	GENERATED_BODY()

/* UPROPERTIES */
public:
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Ref", meta = (AllowPrivateAccess = "true"))
	UCustomGameInstance* GameInstance;
	
private:
	virtual void BeginPlay() override;
	
/* UFUNCTIONS */
public:
	UFUNCTION(BlueprintCallable, Category = "TurnSystem")
    void SwitchPlayerTurn();

private:
	
	
};
