// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit_Child_Tank.h"
#include "Grid.h"
#include "CustomPlayerState.h"
#include "GridPath.h"
#include "GridVisual.h"
#include "TutorialGameMode.h"
#include "UObject/ConstructorHelpers.h"

	// ----------------------------
	// Constructor

AUnit_Child_Tank::AUnit_Child_Tank()
{
	UnitMesh->SetStaticMesh( ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Game_Art/Asset_temp/Character/Paradis/ange_thank/tank.tank'")).Object);
	UnitMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	// Distinction Mesh (temporary)
	
	UnitMeshDistinction->SetStaticMesh( ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'")).Object);
	UnitMeshDistinction->SetCollisionResponseToAllChannels(ECR_Ignore);
	UnitMeshDistinction->SetIsReplicated(true);
	UnitMeshDistinction->SetupAttachment(UnitMesh);
	UnitMeshDistinction->SetRelativeLocation(FVector(0, 0, 90));
	UnitMeshDistinction->SetRelativeScale3D(FVector(.5, .5, .5));

	// Icons
	
	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjectHell(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Tank_Hell.T_Icon_Tank_Hell'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObject(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Tank_Paradise.T_Icon_Tank_Paradise'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjectNeutral(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Tank_Neutral.T_Icon_Tank_Neutral'"));

	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjNeutralDead(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Tank_Neutral_Dead.T_Icon_Tank_Neutral_Dead'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjHeavenDead(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Tank_Paradise_Dead.T_Icon_Tank_Paradise_Dead'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexObjHellDead(TEXT("/Script/Engine.Texture2D'/Game/AssetImport/Textures/UnitIcons/T_Icon_Tank_Hell_Dead.T_Icon_Tank_Hell_Dead'"));
	

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
	// Overrides

void AUnit_Child_Tank::BeginPlay()
{
	Super::BeginPlay();

	if (Attack == 0)
		SetAttack(11);
	if (Defense == 0)
		SetDefense(10);
	if (MaxHealth == 0)
		SetMaxHealth(7);
	if (PM == 0)
		SetPM(4);
	if (CurrentHealth == 0 or CurrentHealth > MaxHealth)
		SetCurrentHealth(MaxHealth);

	UnitName = EUnitName::Tank;
	
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
	
	GetWorld()->GetTimerManager().SetTimer(IconTimerHandle, this, &AUnit_Child_Tank::SetUnitIcon, 1.0f, false);

}

void AUnit_Child_Tank::DisplayWidgetTutorial()
{
	Super::DisplayWidgetTutorial();

	Grid->GridVisual->RemoveStateFromTile(Grid->ConvertLocationToIndex(this->GetActorLocation()), EDC_TileState::Selected);
	if (!GetWorld()->GetAuthGameMode<ATutorialGameMode>()->bIsDead)
		GetWorld()->GetAuthGameMode<ATutorialGameMode>()->DisplayTutorialWidget(1);
}