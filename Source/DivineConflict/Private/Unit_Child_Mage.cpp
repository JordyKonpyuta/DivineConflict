// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit_Child_Mage.h"
#include "Base.h"
#include "CustomGameState.h"
#include "CustomPlayerController.h"
#include "CustomPlayerState.h"
#include "Grid.h"
#include "GridInfo.h"
#include "Projectile.h"
#include "TutorialGameMode.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"

	// ----------------------------
	// Constructor

AUnit_Child_Mage::AUnit_Child_Mage()
{
	UnitMesh->SetStaticMesh( ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Game_Art/Asset_temp/Character/Paradis/ange_mage/mage.mage'")).Object);
	UnitMesh->SetCollisionResponseToAllChannels(ECR_Ignore);


	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjectHell(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Mage_Hell.T_Icon_Mage_Hell'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObject(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Mage_Paradise.T_Icon_Mage_Paradise'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjectNeutral(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Mage_Neutral.T_Icon_Mage_Neutral'"));
	if (IconTexObjectHell.Object != NULL)
		HellIcon = IconTexObjectHell.Object;
	if (IconTexObject.Object != NULL)
		HeavenIcon = IconTexObject.Object;
	if (IconTexObjectNeutral.Object != NULL)
		NeutralIcon = IconTexObjectNeutral.Object;
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
	if (PM == 0)
		SetPM(4);
	
	for (APlayerState* CurrentPlayerState : GetWorld()->GetGameState<ACustomGameState>()->PlayerArray)
	{
		if (ACustomPlayerState* CurrentCustomPlayerState = Cast<ACustomPlayerState>(CurrentPlayerState))
		{
			if (CurrentCustomPlayerState->bIsInTutorial)
			{
				SetCurrentHealth(4);
				if (PlayerOwner == EPlayer::P_Heaven)
					GetWorld()->GetAuthGameMode<ATutorialGameMode>()->Mage = this;
			}
			else if (CurrentHealth == 0 or CurrentHealth > MaxHealth)
				SetCurrentHealth(MaxHealth);
		}
	}

	UnitName = EUnitName::Mage;

	GetWorld()->GetTimerManager().SetTimer(IconTimerHandle, this, &AUnit_Child_Mage::SetUnitIcon, 1.0f, false);

}

void AUnit_Child_Mage::SpecialMage(AActor* Target)
{
	FireBall = GetWorld()->SpawnActor<AProjectile>(AProjectile::StaticClass(), GetActorLocation(), GetActorRotation());
	FireBall->UnitOwner = this;
	FireBall->IsMageAttack = true;
	FireBall->Server_CreateProjectile();
	FireBall->MoveProjectile(Target);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("MAGE SPECIAL"));
	if(ABase* BaseAttack = Cast<ABase>(Target))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("BASE TO ATTACK DAMAGE"));
		BaseAttack->BaseTakeDamage(5);
	}
	if(AUnit* UnitAttack = Cast<AUnit>(Target))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("UNIT TO ATTACK DAMAGE"));
		UnitAttack->SetCurrentHealth(UnitAttack->GetCurrentHealth() - 5);

		if(UnitAttack->GetCurrentHealth() <= 0)
		{
			Grid->GridInfo->RemoveUnitInGrid(UnitAttack);
			PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->SetUnits(PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->GetUnits() - 1);
			GetWorld()->DestroyActor(UnitAttack);
		}
	}
}

void AUnit_Child_Mage::DisplayWidgetTutorial()
{
	Super::DisplayWidgetTutorial();
	GetWorld()->GetAuthGameMode<ATutorialGameMode>()->DisplayTutorialWidget(4);
}
