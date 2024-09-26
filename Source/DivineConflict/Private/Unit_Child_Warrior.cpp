// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit_Child_Warrior.h"
#include "UObject/ConstructorHelpers.h"

void AUnit_Child_Warrior::BeginPlay()
{
	Super::BeginPlay();

	SetAttack(12);
	SetDefense(4);
	SetMaxHealth(11);
	SetCurrentHealth(GetMaxHealth());
	SetPM(5);

	UnitName = EUnitName::Warrior;
}

AUnit_Child_Warrior::AUnit_Child_Warrior()
{
	UnitMesh->SetStaticMesh( ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Game_Art/Asset_temp/Character/Paradis/ange_guerrier/gerrier.gerrier'")).Object);
	UnitMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	if (IsHell)
	{
		static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObject(TEXT("Texture2D'Content/Blueprints/Widget/Assets/Images/Hell/UnitIcons/T_Icon_Warrior_Hell.T_Icon_Warrior_Hell'"));
		if (IconTexObject.Object != NULL)
		{
			UnitIcon = IconTexObject.Object;
		}
	}
	else
	{
		static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObject(TEXT("Texture2D'Content/Blueprints/Widget/Assets/Images/Paradise/UnitIcons/T_Icon_Warrior_Paradise.T_Icon_Warrior_Paradise'"));
		if (IconTexObject.Object != NULL)
		{
			UnitIcon = IconTexObject.Object;
		}
	}
}

void AUnit_Child_Warrior::Special()
{
	Super::Special();
	//Special ability
	SetIsClimbing(true);
}
