// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit_Child_Mage.h"
#include "Base.h"
#include "CustomPlayerController.h"
#include "CustomPlayerState.h"
#include "Grid.h"
#include "GridInfo.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"

	// ----------------------------
	// Constructor

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

	// ----------------------------
	// Replications

void AUnit_Child_Mage::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AUnit_Child_Mage, UnitToAttack);
	DOREPLIFETIME(AUnit_Child_Mage, BaseToAttack);
}

	// ----------------------------
	// Overrides

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
	if (PM == 0)
		SetPM(4);

	UnitName = EUnitName::Mage;
}

void AUnit_Child_Mage::Special()
{
	if(BaseToAttack)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("BASE TO ATTACK DAMAGE"));
		BaseToAttack->TakeDamage(5);
	}
	if(UnitToAttack)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("UNIT TO ATTACK DAMAGE"));
		UnitToAttack->SetCurrentHealth(UnitToAttack->GetCurrentHealth() - 5);

		if(UnitToAttack->GetCurrentHealth() <= 0)
		{
			Grid->GridInfo->RemoveUnitInGrid(UnitToAttack);
			PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->SetUnits(PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->GetUnits() - 1);
			GetWorld()->DestroyActor(UnitToAttack);
		}
	}
}

void AUnit_Child_Mage::PrepareSpecial(FIntPoint SpecialPos)
{
	if(Grid)
		if(Grid->GetGridData()->Find(SpecialPos)->UnitOnTile)
		{
			if(Grid->GetGridData()->Find(SpecialPos)->UnitOnTile->GetPlayerOwner() != PlayerOwner)
			{
				UnitToAttack = Grid->GetGridData()->Find(SpecialPos)->UnitOnTile;
			}
		}
		else
		{
			if(Grid->GetGridData()->Find(SpecialPos)->BaseOnTile)
			{
				if(Grid->GetGridData()->Find(SpecialPos)->BaseOnTile->PlayerOwner != PlayerOwner)
				{
					BaseToAttack = Grid->GetGridData()->Find(SpecialPos)->BaseOnTile;
				}
			}
		}
}