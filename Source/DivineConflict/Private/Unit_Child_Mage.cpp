// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit_Child_Mage.h"

#include "Base.h"
#include "CustomPlayerState.h"
#include "Grid.h"
#include "GridInfo.h"
#include "UObject/ConstructorHelpers.h"

void AUnit_Child_Mage::BeginPlay()
{
	Super::BeginPlay();

	if (Attack == 0)
		SetAttack(5);
	if (Defense == 0)
		SetDefense(2);
	if (MaxHealth == 0)
		SetMaxHealth(10);
	if (CurrentHealth == 0 or CurrentHealth > MaxHealth)
		SetCurrentHealth(MaxHealth);
	if (PM == 3)
		SetPM(4);

	UnitName = EUnitName::Mage;
}

AUnit_Child_Mage::AUnit_Child_Mage()
{
	UnitMesh->SetStaticMesh( ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Game_Art/Asset_temp/Character/Paradis/ange_mage/mage.mage'")).Object);
	UnitMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

		static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjectHell(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Mage_Hell.T_Icon_Mage_Hell'"));
		if (IconTexObjectHell.Object != NULL)
		{
			UnitIconHell = IconTexObjectHell.Object;
		}

		static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObject(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Mage_Paradise.T_Icon_Mage_Paradise'"));
		if (IconTexObject.Object != NULL)
		{
			UnitIconParadise = IconTexObject.Object;
		}
}

void AUnit_Child_Mage::SpecialUnit(AUnit* UnitToAttack)
{
	Super::SpecialUnit(UnitToAttack);
	UnitToAttack->SetCurrentHealth(UnitToAttack->GetCurrentHealth() - 5);

	if(UnitToAttack->GetCurrentHealth() <= 0)
	{
		Grid->GridInfo->RemoveUnitInGrid(UnitToAttack);
		PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->SetUnits(PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->GetUnits() - 1);
		GetWorld()->DestroyActor(UnitToAttack);
	}
}

void AUnit_Child_Mage::SpecialBase(ABase* BaseToAttack)
{
	Super::SpecialBase(BaseToAttack);
	BaseToAttack->TakeDamage(5);
}




