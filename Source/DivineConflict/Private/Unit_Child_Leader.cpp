// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit_Child_Leader.h"

#include "UObject/ConstructorHelpers.h"

void AUnit_Child_Leader::BeginPlay()
{
	Super::BeginPlay();

	SetAttack(7);
	SetDefense(0);
	SetMaxHealth(20);
	SetCurrentHealth(GetMaxHealth());
	SetPM(3);
}

AUnit_Child_Leader::AUnit_Child_Leader()
{
	if (IsHell)
	{
		static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObject(TEXT("Texture2D'Content/Blueprints/Widget/Assets/Images/Hell/UnitIcons/T_Icon_Leader_Hell.T_Icon_Leader_Hell'"));
		if (IconTexObject.Object != NULL)
		{
			UnitIcon = IconTexObject.Object;
		}
	}
	else
	{
		static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObject(TEXT("Texture2D'Content/Blueprints/Widget/Assets/Images/Paradise/UnitIcons/T_Icon_Leader_Paradise.T_Icon_Leader_Paradise'"));
		if (IconTexObject.Object != NULL)
		{
			UnitIcon = IconTexObject.Object;
		}
	}
	
}
