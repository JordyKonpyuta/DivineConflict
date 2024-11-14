// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit_Child_Tank.h"

#include "CustomGameState.h"
#include "Grid.h"
#include "CustomPlayerState.h"
#include "GridPath.h"
#include "GridVisual.h"
#include "TutorialGameMode.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"

	// ----------------------------
	// Constructor

AUnit_Child_Tank::AUnit_Child_Tank()
{
	UnitMesh->SetStaticMesh( ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Game_Art/Asset_temp/Character/Paradis/ange_thank/tank.tank'")).Object);
	UnitMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	


	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjectHell(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Tank_Hell.T_Icon_Tank_Hell'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObject(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Tank_Paradise.T_Icon_Tank_Paradise'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjectNeutral(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Tank_Neutral.T_Icon_Tank_Neutral'"));
	if (IconTexObjectHell.Object != NULL)
		HellIcon = IconTexObjectHell.Object;
	if (IconTexObject.Object != NULL)
		HeavenIcon = IconTexObject.Object;
	if (IconTexObjectNeutral.Object != NULL)
		NeutralIcon = IconTexObjectNeutral.Object;

}

void AUnit_Child_Tank::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUnit_Child_Tank, bIsUsingSpecial);
}




void AUnit_Child_Tank::SetTimer()
{
	
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
	if (PM == 0)
		SetPM(3);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AUnit_Child_Tank::SetTimer, 1.0f, true);
	for (APlayerState* CurrentPlayerState : GetWorld()->GetGameState<ACustomGameState>()->PlayerArray)
	{
		if (ACustomPlayerState* CurrentCustomPlayerState = Cast<ACustomPlayerState>(CurrentPlayerState))
		{
			if (CurrentCustomPlayerState->bIsInTutorial)
			{
				SetCurrentHealth(4);
				if (PlayerOwner == EPlayer::P_Heaven)
					GetWorld()->GetAuthGameMode<ATutorialGameMode>()->Tank = this;
			}
			else if (CurrentHealth == 0 or CurrentHealth > MaxHealth)
				SetCurrentHealth(MaxHealth);
		}
	}
	UnitName = EUnitName::Tank;

	GetWorld()->GetTimerManager().SetTimer(IconTimerHandle, this, &AUnit_Child_Tank::SetUnitIcon, 1.0f, false);

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
			SetIsUsingSpecial(true);
			Grid->GetGridData()->Find(Tile)->UnitOnTile->Server_GetBuffs();
		}
	}
}

void AUnit_Child_Tank::DisplayWidgetTutorial()
{
	Super::DisplayWidgetTutorial();

	Grid->GridVisual->RemoveStateFromTile(Grid->ConvertLocationToIndex(this->GetActorLocation()), EDC_TileState::Selected);
	if (!GetWorld()->GetAuthGameMode<ATutorialGameMode>()->isDead)
		GetWorld()->GetAuthGameMode<ATutorialGameMode>()->DisplayTutorialWidget(1);
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
