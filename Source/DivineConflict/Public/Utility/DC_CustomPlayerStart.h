// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumsList.h"
#include "GameFramework/PlayerStart.h"
#include "DC_CustomPlayerStart.generated.h"


/**
 * 
 */
UCLASS()
class DIVINECONFLICT_API ADC_CustomPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivate))
	EPlayer PlayerTeam;
	
};
