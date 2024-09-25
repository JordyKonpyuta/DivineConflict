// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit_Child_Tank.h"
#include "UObject/ConstructorHelpers.h"

void AUnit_Child_Tank::BeginPlay()
{
	Super::BeginPlay();

	SetAttack(10);
	SetDefense(9);
	SetMaxHealth(12);
	SetCurrentHealth(GetMaxHealth());
	SetPM(3);
}

AUnit_Child_Tank::AUnit_Child_Tank()
{

	UnitMesh->SetStaticMesh( ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Game_Art/Asset_temp/Character/Paradis/ange_thank/tank.tank'")).Object);
	UnitMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	if (IsHell)
	{
		static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObject(TEXT("Texture2D'Content/Blueprints/Widget/Assets/Images/Hell/UnitIcons/T_Icon_Tank_Hell.T_Icon_Tank_Hell'"));
		if (IconTexObject.Object != NULL)
		{
			UnitIcon = IconTexObject.Object;
		}
	}
	else
	{
		static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObject(TEXT("Texture2D'Content/Blueprints/Widget/Assets/Images/Paradise/UnitIcons/T_Icon_Tank_Paradise.T_Icon_Tank_Paradise'"));
		if (IconTexObject.Object != NULL)
		{
			UnitIcon = IconTexObject.Object;
		}
	}
}
