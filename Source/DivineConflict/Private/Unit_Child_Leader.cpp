// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit_Child_Leader.h"

#include "Grid.h"
#include "GridPath.h"
#include "UObject/ConstructorHelpers.h"

void AUnit_Child_Leader::BeginPlay()
{
	Super::BeginPlay();

	SetAttack(7);
	SetDefense(0);
	SetMaxHealth(20);
	SetCurrentHealth(GetMaxHealth());
	SetPM(3);

	UnitName = EUnitName::Leader;
}

AUnit_Child_Leader::AUnit_Child_Leader()
{
	UnitMesh->SetStaticMesh( ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Game_Art/Asset_temp/Character/Paradis/Ange_test_shape/SM_ange_mage_Scale_up_full_float_1_2.SM_ange_mage_Scale_up_full_float_1_2'")).Object);
	UnitMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

		static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjectHell(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/UnitIcons/T_Icon_Leader_Hell.T_Icon_Leader_Hell'"));
		if (IconTexObjectHell.Object != NULL)
		{
			UnitIconHell = IconTexObjectHell.Object;
		}

		static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObject(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/UnitIcons/T_Icon_Leader_Paradise.T_Icon_Leader_Paradise'"));
		if (IconTexObject.Object != NULL)
		{
			UnitIconParadise = IconTexObject.Object;
		}

	
}

void AUnit_Child_Leader::Special()
{
	Super::Special();
	//Special ability

	TArray<FIntPoint> PathBuff =  Grid->GridPath->FindPath(GetIndexPosition(),FIntPoint(-999,-999),true,3,false);
	for(FIntPoint Tile : PathBuff)
	{
		if(Grid->GetGridData()->Find(Tile)->UnitOnTile  && Grid->GetGridData()->Find(Tile)->UnitOnTile != this && Grid->GetGridData()->Find(Tile)->UnitOnTile->GetPlayerOwner() == GetPlayerOwner())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Buff Leader"));
			//Grid->GetGridData()->Find(Tile)->UnitOnTile->SetBuffLeader(true);
		}
	}
}
