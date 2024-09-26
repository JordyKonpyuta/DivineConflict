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

		static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjHell(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Warrior_Hell.T_Icon_Warrior_Hell'"));
		if (IconTexObjHell.Object)
		{
			UnitIconHell = IconTexObjHell.Object;
		}
	
		static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObj(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Warrior_Paradise.T_Icon_Warrior_Paradise'"));
		if (IconTexObj.Object)
		{
			UnitIconParadise = IconTexObj.Object;
		}

}

void AUnit_Child_Warrior::Special()
{
	Super::Special();
	//Special ability
	SetIsClimbing(true);
}
