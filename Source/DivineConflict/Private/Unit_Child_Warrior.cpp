// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit_Child_Warrior.h"

#include "CameraPlayer.h"
#include "Upwall.h"
#include "CustomGameState.h"
#include "CustomPlayerController.h"
#include "Grid.h"
#include "GridVisual.h"
#include "TutorialGameMode.h"
#include "GameFramework/CharacterMovementReplication.h"
#include "Kismet/KismetSystemLibrary.h"
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
	if (PM == 0)
		SetPM(5);
	for (APlayerState* CurrentPlayerState : GetWorld()->GetGameState<ACustomGameState>()->PlayerArray)
	{
		if (ACustomPlayerState* CurrentCustomPlayerState = Cast<ACustomPlayerState>(CurrentPlayerState))
		{
			if (CurrentCustomPlayerState->bIsInTutorial)
			{
				SetCurrentHealth(6);
				if (UKismetSystemLibrary::GetDisplayName(this) == "HeavenWarrior1")
					GetWorld()->GetAuthGameMode<ATutorialGameMode>()->Warrior1 = this;
				else if (UKismetSystemLibrary::GetDisplayName(this) == "HeavenWarrior2")
					GetWorld()->GetAuthGameMode<ATutorialGameMode>()->Warrior2 = this;
			}
			else if (CurrentHealth == 0 or CurrentHealth > MaxHealth)
				SetCurrentHealth(MaxHealth);
		}
	}

	UnitName = EUnitName::Warrior;

}

void AUnit_Child_Warrior::DisplayWidgetTutorial()
{
	Super::DisplayWidgetTutorial();

	Grid->GridVisual->RemoveStateFromTile(Grid->ConvertLocationToIndex(this->GetActorLocation()), EDC_TileState::Selected);
	if (UKismetSystemLibrary::GetDisplayName(this) == "HeavenWarrior1")
		GetWorld()->GetAuthGameMode<ATutorialGameMode>()->DisplayTutorialWidget(3);
	else if (UKismetSystemLibrary::GetDisplayName(this) == "HeavenWarrior2")
		GetWorld()->GetAuthGameMode<ATutorialGameMode>()->DisplayTutorialWidget(6);
}


void AUnit_Child_Warrior::Special()
{
	Super::Special();
	//Special ability
	if (HasMoved)
	{
		if (AUpwall* WallToClimb = Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(GetFinalGhostMesh()->GetComponentLocation()))->UpwallOnTile)
		{
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
			if (WallToClimb->GetClimbLocation() != FIntPoint(-999, -999))
			{
				TArray<FIntPoint> NewMove;
				NewMove.Add(WallToClimb->GetClimbLocation());
				SetIsSelected(false);
				PlayerControllerRef->Server_PrepareMoveUnit(NewMove, this);
				FutureMovement[0] = WallToClimb->GetClimbLocation();
				GhostsFinaleLocationMesh->SetWorldLocation(Grid->ConvertIndexToLocation(WallToClimb->GetClimbLocation()));
			}
		}
	}
}
