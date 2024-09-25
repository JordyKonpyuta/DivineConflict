// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit_Child_Mage.h"
#include "UObject/ConstructorHelpers.h"

void AUnit_Child_Mage::BeginPlay()
{
	Super::BeginPlay();

	SetAttack(5);
	SetDefense(2);
	SetMaxHealth(10);
	SetCurrentHealth(GetMaxHealth());
	SetPM(4);
}

AUnit_Child_Mage::AUnit_Child_Mage()
{
	UnitMesh->SetStaticMesh( ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Game_Art/Asset_temp/Character/Paradis/ange_mage/mage.mage'")).Object);
	UnitMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	if (IsHell)
	{
		static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObject(TEXT("Texture2D'Content/Blueprints/Widget/Assets/Images/Hell/UnitIcons/T_Icon_Mage_Hell.T_Icon_Mage_Hell'"));
		if (IconTexObject.Object != NULL)
		{
			UnitIcon = IconTexObject.Object;
		}
	}
	else
	{
		static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObject(TEXT("Texture2D'Content/Blueprints/Widget/Assets/Images/Paradise/UnitIcons/T_Icon_Mage_Paradise.T_Icon_Mage_Paradise'"));
		if (IconTexObject.Object != NULL)
		{
			UnitIcon = IconTexObject.Object;
		}
	}
}
