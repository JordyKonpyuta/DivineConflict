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
#include "Engine/DamageEvents.h"

#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"

	// ----------------------------
	// Constructor

AUnit_Child_Mage::AUnit_Child_Mage()
{
	UnitMesh->SetStaticMesh( ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Game_Art/Asset_temp/Character/Paradis/ange_mage/mage.mage'")).Object);
	UnitMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	// Distinction Mesh (temporary)
	
	UnitMeshDistinction->SetStaticMesh( ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'")).Object);
	UnitMeshDistinction->SetCollisionResponseToAllChannels(ECR_Ignore);
	UnitMeshDistinction->SetIsReplicated(true);
	UnitMeshDistinction->SetupAttachment(UnitMesh);
	UnitMeshDistinction->SetRelativeLocation(FVector(0, 0, 90));
	UnitMeshDistinction->SetRelativeScale3D(FVector(.5, .5, .5));


	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjectHell(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Mage_Hell.T_Icon_Mage_Hell'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObject(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Mage_Paradise.T_Icon_Mage_Paradise'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjectNeutral(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Mage_Neutral.T_Icon_Mage_Neutral'"));

	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjNeutralDead(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Mage_Neutral_Dead.T_Mage_Neutral_Dead'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjHeavenDead(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Mage_Paradise_Dead.T_Mage_Paradise_Dead'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjHellDead(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Mage_Hell_Dead.T_Mage_Hell_Dead'"));
	
	if (IconTexObjectHell.Object != NULL)
		HellIcon = IconTexObjectHell.Object;
	if (IconTexObject.Object != NULL)
		HeavenIcon = IconTexObject.Object;
	if (IconTexObjectNeutral.Object != NULL)
		NeutralIcon = IconTexObjectNeutral.Object;
	if (IconTexObjNeutralDead.Object != NULL)
		NeutralIconDead = IconTexObjNeutralDead.Object;
	if (IconTexObjHeavenDead.Object != NULL)
		HeavenIconDead = IconTexObjHeavenDead.Object;
	if (IconTexObjHellDead.Object != NULL)
		HellIconDead = IconTexObjHellDead.Object;
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
		SetPM(5);
	
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
	
	switch (PlayerOwner)
	{
	case EPlayer::P_Hell:
		UnitMeshDistinction->SetMaterial(0, AllMaterials[2]);
		break;
	case EPlayer::P_Heaven:
		UnitMeshDistinction->SetMaterial(0, AllMaterials[1]);
		break;
	case EPlayer::P_Neutral:
		UnitMeshDistinction->SetMaterial(0, AllMaterials[0]);
		break;
	}
}

void AUnit_Child_Mage::SpecialMage(AActor* Target)
{
	if (Target)
	{
		float NewRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation()).Yaw;
		NewRot = UKismetMathLibrary::GridSnap_Float(NewRot, 45) - 90;
		UnitRotation = FRotator(0, NewRot, 0);
		
		FireBall = GetWorld()->SpawnActor<AProjectile>(AProjectile::StaticClass(), GetActorLocation(), GetActorRotation());
		FireBall->UnitOwner = this;
		FireBall->IsMageAttack = true;
		FireBall->Server_CreateProjectile();
		FireBall->MoveProjectile(Target);
		if(ABase* BaseAttack = Cast<ABase>(Target))
		{
			FDamageEvent DamageEvent;
			BaseAttack->TakeDamage(5.f, DamageEvent, nullptr, this);
		}
		if(AUnit* UnitAttack = Cast<AUnit>(Target))
		{
			FDamageEvent DamageEvent;
			UnitAttack->TakeDamage(UnitAttack->GetDefense() + 5, DamageEvent, nullptr, this);

			if(UnitAttack->GetCurrentHealth() <= 0)
			{
				if (UnitAttack->GetIsGarrison())
				{
					UnitAttack->GetBuildingRef()->UnitRef = nullptr;
					UnitAttack->GetBuildingRef()->GarrisonFull = false;
					UnitAttack->SetIsGarrison(false);
				}
				Grid->GridInfo->RemoveUnitInGrid(UnitAttack);
				PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->SetUnits(PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->GetUnits() - 1);
				GetWorld()->DestroyActor(UnitAttack);
			}
		}
	}
}

void AUnit_Child_Mage::DisplayWidgetTutorial()
{
	Super::DisplayWidgetTutorial();
	if (!GetWorld()->GetAuthGameMode<ATutorialGameMode>()->isDead)
		GetWorld()->GetAuthGameMode<ATutorialGameMode>()->DisplayTutorialWidget(4);
}
