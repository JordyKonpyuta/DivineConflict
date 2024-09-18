// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CustomGameInstance.generated.h"

class ACustomPlayerController;

/**
 * 
 */
UCLASS()
class DIVINECONFLICT_API UCustomGameInstance : public UGameInstance
{
	GENERATED_BODY()

/* UPROPERTIES */
public:
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "PlayerControllers", meta = (AllowPrivateAccess = "true"))
	TArray<ACustomPlayerController*> PlayerControllers;

private:

	
/* UFUNCTIONS */	
public:

	
private:
};
