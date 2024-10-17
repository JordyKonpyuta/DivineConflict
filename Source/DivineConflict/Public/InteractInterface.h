// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

class ACustomPlayerController;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DIVINECONFLICT_API IInteractInterface
{
	GENERATED_BODY()

	// UPROPERTIES
public:

protected:

	// UFUNCTIONS
public:
	// ----------------------------
	// Master Interaction
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Interface)
	bool Interact(ACustomPlayerController *PlayerController);

protected:
};
