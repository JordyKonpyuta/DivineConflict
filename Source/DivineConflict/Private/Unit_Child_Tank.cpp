// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit_Child_Tank.h"

#include "CustomGameState.h"
#include "Grid.h"
#include "GridPath.h"
#include "UObject/ConstructorHelpers.h"

	// ----------------------------
	// Constructor

AUnit_Child_Tank::AUnit_Child_Tank()
{
	UnitMesh->SetStaticMesh( ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Game_Art/Asset_temp/Character/Paradis/ange_thank/tank.tank'")).Object);
	UnitMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	

	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjectHell(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Tank_Hell.T_Icon_Tank_Hell'"));
	if (IconTexObjectHell.Object != NULL)
	{
		UnitIconHell = IconTexObjectHell.Object;
	}


	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObject(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Tank_Paradise.T_Icon_Tank_Paradise'"));
	if (IconTexObject.Object != NULL)
	{
		UnitIconParadise = IconTexObject.Object;
	}
}

	// ----------------------------
	// Overrides

void AUnit_Child_Tank::BeginPlay()
{
	Super::BeginPlay();

	if (Attack == 0)
		SetAttack(10);
	if (Defense == 0)
		SetDefense(9);
	if (MaxHealth == 0)
		SetMaxHealth(12);
	if (CurrentHealth == 0 or CurrentHealth > MaxHealth)
		SetCurrentHealth(MaxHealth);
	if (PM == 0)
		SetPM(3);

	UnitName = EUnitName::Tank;

	for (APlayerState* CurrentPlayerState : GetWorld()->GetGameState<ACustomGameState>()->PlayerArray)
	{
		if (ACustomPlayerState* CurrentCustomPlayerState = Cast<ACustomPlayerState>(CurrentPlayerState))
		{
			if (CurrentCustomPlayerState->PlayerTeam == PlayerOwner)
			{
				CurrentCustomPlayerState->SetTankCount(CurrentCustomPlayerState->GetTankCount() + 1);
			}
		}
	}
}

void AUnit_Child_Tank::Special()
{
	Super::Special();
	//Special ability

	TArray<FIntPoint> PathBuff =  Grid->GridPath->FindTileNeighbors(GetIndexPosition());
	for(FIntPoint Tile : PathBuff)
	{
		if(Grid->GetGridData()->Find(Tile)->UnitOnTile && Grid->GetGridData()->Find(Tile)->UnitOnTile != this && Grid->GetGridData()->Find(Tile)->UnitOnTile->GetPlayerOwner() == GetPlayerOwner())
		{
			Grid->GetGridData()->Find(Tile)->UnitOnTile->SetBuffTank(true);
		}
	}
}


	// ----------------------------
	// GETTERS

bool AUnit_Child_Tank::GetIsUsingSpecial()
{
	return bIsUsingSpecial;
}

	// ----------------------------
	// SETTERS

void AUnit_Child_Tank::SetIsUsingSpecial(bool bIsBoosting)
{
	bIsUsingSpecial = bIsBoosting;
}
