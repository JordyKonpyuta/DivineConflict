// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CustomGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class DIVINECONFLICT_API UCustomGameInstance : public UGameInstance
{
	GENERATED_BODY()

/* UPROPERTIES */
public:
	

private:

	
/* UFUNCTIONS */	
public:

	UCustomGameInstance();

	UFUNCTION(BlueprintCallable, Category = "CustomGameInstance")
	void EnableFaceBottomKey(bool enable);
	
private:
};
