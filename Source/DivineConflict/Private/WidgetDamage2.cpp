// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetDamage2.h"

	// ----------------------------
	// Overrides

void UWidgetDamage2::NativeConstruct()
{
	Super::NativeConstruct();
}

void UWidgetDamage2::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	
}

	// ----------------------------
	// Damage

void UWidgetDamage2::ChangeTextDmg_Implementation(int Dmg)
{
	return;
}
