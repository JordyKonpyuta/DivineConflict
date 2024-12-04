// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit_Child_Warrior.h"
#include "CustomPlayerController.h"
#include "Grid.h"
#include "GridVisual.h"
#include "TutorialGameMode.h"
#include "UObject/ConstructorHelpers.h"

	// ----------------------------
	// Constructor

AUnit_Child_Warrior::AUnit_Child_Warrior()
{
	SetReplicates(true);
	
	UnitMesh->SetStaticMesh( ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Game_Art/Asset_temp/Character/Paradis/ange_guerrier/gerrier.gerrier'")).Object);
	UnitMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	// Distinction Mesh (temporary)
	
	UnitMeshDistinction->SetStaticMesh( ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cone.Cone'")).Object);
	UnitMeshDistinction->SetCollisionResponseToAllChannels(ECR_Ignore);
	UnitMeshDistinction->SetIsReplicated(true);
	UnitMeshDistinction->SetupAttachment(UnitMesh);
	UnitMeshDistinction->SetRelativeLocation(FVector(0, 0, 90));
	UnitMeshDistinction->SetRelativeScale3D(FVector(.5, .5, .5));
		
	// Icons
	
	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjHell(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Warrior_Hell.T_Icon_Warrior_Hell'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObj(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Warrior_Paradise.T_Icon_Warrior_Paradise'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjNeutral(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Warrior_Neutral.T_Icon_Warrior_Neutral'"));

	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjNeutralDead(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Warrior_Neutral_Dead.T_Icon_Warrior_Neutral_Dead'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjHeavenDead(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Warrior_Paradise_Dead.T_Icon_Warrior_Paradise_Dead'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjHellDead(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Warrior_Hell_Dead.T_Icon_Warrior_Hell_Dead'"));
	
	if (IconTexObjHell.Object != NULL)
		HellIcon = IconTexObjHell.Object;
	if (IconTexObj.Object != NULL)
		HeavenIcon = IconTexObj.Object;
	if (IconTexObjNeutral.Object != NULL)
		NeutralIcon = IconTexObjNeutral.Object;
	if (IconTexObjNeutralDead.Object != NULL)
		NeutralIconDead = IconTexObjNeutralDead.Object;
	if (IconTexObjHeavenDead.Object != NULL)
		HeavenIconDead = IconTexObjHeavenDead.Object;
	if (IconTexObjHellDead.Object != NULL)
		HellIconDead = IconTexObjHellDead.Object;
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
		SetPM(6);
	if (CurrentHealth == 0 or CurrentHealth > MaxHealth)
		SetCurrentHealth(MaxHealth);

	UnitName = EUnitName::Warrior;

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
	
	GetWorld()->GetTimerManager().SetTimer(IconTimerHandle, this, &AUnit_Child_Warrior::SetUnitIcon, 1.0f, false);

}

void AUnit_Child_Warrior::DisplayWidgetTutorial()
{
	Super::DisplayWidgetTutorial();

	Grid->GridVisual->RemoveStateFromTile(Grid->ConvertLocationToIndex(this->GetActorLocation()), EDC_TileState::Selected);
	if (GetWorld()->GetAuthGameMode<ATutorialGameMode>()->Warrior1 == this && !GetWorld()->GetAuthGameMode<ATutorialGameMode>()->bIsDead)
		GetWorld()->GetAuthGameMode<ATutorialGameMode>()->DisplayTutorialWidget(3);
	else if (GetWorld()->GetAuthGameMode<ATutorialGameMode>()->Warrior2 == this && !GetWorld()->GetAuthGameMode<ATutorialGameMode>()->bIsDead)
		GetWorld()->GetAuthGameMode<ATutorialGameMode>()->DisplayTutorialWidget(6);
}