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
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"

	// ----------------------------
	// Constructor

AUnit_Child_Warrior::AUnit_Child_Warrior()
{
	SetReplicates(true);
	
	UnitMesh->SetStaticMesh( ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Game_Art/Asset_temp/Character/Paradis/ange_guerrier/gerrier.gerrier'")).Object);
	UnitMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjHell(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Warrior_Hell.T_Icon_Warrior_Hell'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObj(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Warrior_Paradise.T_Icon_Warrior_Paradise'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjNeutral(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Warrior_Paradise.T_Icon_Warrior_Neutral'"));
	if (IconTexObjHell.Object)
		HellIcon = IconTexObjHell.Object;
	if (IconTexObj.Object)
		HeavenIcon = IconTexObj.Object;
	if (IconTexObjNeutral.Object)
		NeutralIcon = IconTexObjNeutral.Object;
	

}
void AUnit_Child_Warrior::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUnit_Child_Warrior, WallToClimb);
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

	GetWorld()->GetTimerManager().SetTimer(IconTimerHandle, this, &AUnit_Child_Warrior::SetUnitIcon, 1.0f, false);

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

	//Special ability
	if (HasMoved)
	{
		WallToClimb = Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(GetFinalGhostMesh()->GetComponentLocation()))->UpwallOnTile;
		if (WallToClimb)
		{
			if (WallToClimb->GetClimbLocation() != FIntPoint(-999, -999))
			{
				FutureMovementWithSpecial.AddUnique(WallToClimb->GetClimbLocation());
				Server_SpecialMove(WallToClimb->GetClimbLocation());
				GhostsFinaleLocationMesh->SetWorldLocation(Grid->ConvertIndexToLocation(WallToClimb->GetClimbLocation()));
				WallToClimb = nullptr;
				InitGhosts();
			}
		}
	}
	else
	{
		WallToClimb = Grid->GetGridData()->Find(GetIndexPosition())->UpwallOnTile;
		if (WallToClimb)
		{
			if (WallToClimb->GetClimbLocation() != FIntPoint(-999, -999)){
				const TArray<FIntPoint> NewMove;
				
				FutureMovementWithSpecial.AddUnique(WallToClimb->GetClimbLocation());
				Server_SpecialMove(WallToClimb->GetClimbLocation());
				PlayerControllerRef->Server_ActivateSpecial(this, WallToClimb->GetClimbLocation());
				SetIsSelected(false);
				//PlayerControllerRef->Server_PrepareMoveUnit(NewMove, this);
				//FutureMovement.Add(WallToClimb->GetClimbLocation());
				GhostsFinaleLocationMesh->SetWorldLocation(Grid->ConvertIndexToLocation(WallToClimb->GetClimbLocation()));
				InitGhosts();
			}
		}
	}
}

void AUnit_Child_Warrior::Server_SpecialMove_Implementation(FIntPoint NewPos)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("1")));
		
	Multi_SpecialMove(NewPos);
}

void AUnit_Child_Warrior::Multi_SpecialMove_Implementation(FIntPoint NewPos)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("2")));
	FutureMovementWithSpecial.AddUnique(NewPos);
	if(HasMoved)
		FirstActionIsMove = true;
}

void AUnit_Child_Warrior::Server_MoveToClimb_Implementation()
{
	Multi_MoveToClimb();
}

void AUnit_Child_Warrior::Multi_MoveToClimb_Implementation()
{

	if (FirstActionIsMove)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("FirstActionIsMove = %d"), FirstActionIsMove));
		if(!FutureMovementWithSpecial.IsEmpty())
		{
			if(FutureMovementWithSpecial.Last() == IndexPosition)
			{
				if(Grid->GetGridData()->Find(FutureMovementWithSpecial.Last())->UpwallOnTile)
				{
					FutureMovementWithSpecial.AddUnique(Grid->GetGridData()->Find(FutureMovementWithSpecial.Last())->UpwallOnTile->GetClimbLocation());
				}
			}
			InitializeFullMove(TArray<FIntPoint>{FutureMovementWithSpecial.Last()});
			FutureMovementWithSpecial.Empty();
			Server_GetBuffs();
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("FirstActionIsMove = %d"), FirstActionIsMove));
		TArray<FIntPoint> TempFullMoveSave = FutureMovement;
		FutureMovement = FutureMovementWithSpecial;
		InitializeFullMove(TArray<FIntPoint> {FutureMovement[0]});
		FutureMovementWithSpecial.Empty();
		FutureMovement = TempFullMoveSave;
		Server_GetBuffs();
	}
		
	
}

