// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit_Child_Warrior.h"
#include "Upwall.h"
#include "CustomGameState.h"
#include "Grid.h"
#include "GameFramework/CharacterMovementReplication.h"
#include "UObject/ConstructorHelpers.h"

	// ----------------------------
	// Constructor

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

	// ----------------------------
	// Overrides

void AUnit_Child_Warrior::BeginPlay()
{
	Super::BeginPlay();

	if (Attack == 0)
		SetAttack(12);
	if (Defense == 0)
		SetDefense(4);
	if (MaxHealth == 0)
		SetMaxHealth(11);
	if (CurrentHealth == 0 or CurrentHealth > MaxHealth )
		SetCurrentHealth(MaxHealth);
	if (PM == 0)
		SetPM(5);

	UnitName = EUnitName::Warrior;
	
	for (APlayerState* CurrentPlayerState : GetWorld()->GetGameState<ACustomGameState>()->PlayerArray)
	{
		if (ACustomPlayerState* CurrentCustomPlayerState = Cast<ACustomPlayerState>(CurrentPlayerState))
		{
			if (CurrentCustomPlayerState->PlayerTeam == PlayerOwner)
			{
				CurrentCustomPlayerState->SetWarriorCount(CurrentCustomPlayerState->GetWarriorCount() + 1);
				CurrentCustomPlayerState->SetPopulation();
			}
		}
	}
}

void AUnit_Child_Warrior::Special()
{
	Super::Special();
	//Special ability
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Special Activatated"));
	if (HasMoved)
	{
		if (AUpwall* WallToClimb = Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(GetFinalGhostMesh()->GetComponentLocation()))->UpwallOnTile)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("UpwallToClimb found"));
			TArray<FIntPoint> NewMove;
			NewMove.Add(WallToClimb->GetClimbLocation());
			if (WallToClimb->GetClimbLocation() != FIntPoint(-999, -999))
			{
				FutureMovement.Add(WallToClimb->GetClimbLocation());
				GhostsFinaleLocationMesh->SetWorldLocation(Grid->ConvertIndexToLocation(WallToClimb->GetClimbLocation()));
			}
		}
	}
	else
	{
		if (AUpwall* WallToClimb = Grid->GetGridData()->Find(GetIndexPosition())->UpwallOnTile)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("UpwallToClimb found"));
			TArray<FIntPoint> NewMove;
			NewMove.Add(WallToClimb->GetClimbLocation());
			if (WallToClimb->GetClimbLocation() != FIntPoint(-999, -999))
			{
				FutureMovement.Add(WallToClimb->GetClimbLocation());
				GhostsFinaleLocationMesh->SetWorldLocation(Grid->ConvertIndexToLocation(WallToClimb->GetClimbLocation()));
			}
		}
	}
}
