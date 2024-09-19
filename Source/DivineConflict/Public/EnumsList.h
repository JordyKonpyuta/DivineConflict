// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EnumsList.generated.h"

/**
 * 
 */
UCLASS()
class DIVINECONFLICT_API UEnumsList : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};

UENUM(BlueprintType)
enum class EBuildingList : uint8 {
	B_Wood	= 0		UMETA(DisplayName="Wood"),
	B_Stone	= 1		UMETA(DisplayName="Stone"),
	B_Gold	= 2		UMETA(DisplayName="Gold"),
	B_AP	= 3		UMETA(DisplayName="AP")
};

UENUM(BlueprintType)
enum class EUnitType : uint8
{
	U_Warrior	= 0		UMETA(DisplayName="Warrior"),
	U_Tank		= 1		UMETA(DisplayName="Tank"),
	U_Mage		= 2		UMETA(DisplayName="Mage"),
	U_Leader	= 3		UMETA(DisplayName="Leader")
};

UENUM(BlueprintType)
enum class EPlayer : uint8
{
	P_Hell		= 0		UMETA(DisplayName="HellPlayers"),
	P_Heaven	= 1		UMETA(DisplayName="HeavenPlayers"),
	P_Neutral	= 2		UMETA(DisplayName="Neutral"),
};