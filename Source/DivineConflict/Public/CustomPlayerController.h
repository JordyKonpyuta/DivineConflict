// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CustomPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DIVINECONFLICT_API ACustomPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	UPROPERTY()
	bool IsHell = false;

public:
	UFUNCTION(BlueprintCallable)
	bool GetIsHell();

	UFUNCTION(BlueprintCallable)
	void SetIsHell(bool h);
	
};
