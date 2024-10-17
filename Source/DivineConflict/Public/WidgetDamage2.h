// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetDamage2.generated.h"

/**
 * 
 */
UCLASS()
class DIVINECONFLICT_API UWidgetDamage2 : public UUserWidget
{
	GENERATED_BODY()
	
// UPROPERTIES
public:
	// ----------------------------
	// Damage
	
	UPROPERTY(BlueprintReadOnly)
	int DamageTaken;

private:

// UFUNCTIONS
public:
	// ----------------------------
	// Damage
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ChangeTextDmg(int Dmg);

protected:
	// ----------------------------
	// Overrides

	void NativeConstruct() override;
	
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
