// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/GhostUnitSpawning.h"

// Sets default values
AGhostUnitSpawning::AGhostUnitSpawning()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	MaterialToGhosts = ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("/Script/Engine.Material'/Game/Core/Texture/M_Ghosts.M_Ghosts'")).Object;
	Mesh->SetMaterial(0, MaterialToGhosts);

	MeshUnits.Add(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Game_Art/Asset_temp/Character/Paradis/ange_thank/tank.tank'")).Object);
	MeshUnits.Add(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Game_Art/Asset_temp/Character/Paradis/ange_guerrier/gerrier.gerrier'")).Object);
	MeshUnits.Add(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Game_Art/Asset_temp/Character/Paradis/ange_mage/mage.mage'")).Object);
	MeshUnits.Add(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Game_Art/Asset_temp/Character/Paradis/Ange_test_shape/SM_ange_mage_Scale_up_full_float_1_2.SM_ange_mage_Scale_up_full_float_1_2'")).Object);
	

}

// Called when the game starts or when spawned
void AGhostUnitSpawning::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void AGhostUnitSpawning::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

EUnitType AGhostUnitSpawning::GetUnitType()
{
	return UnitType;
}

void AGhostUnitSpawning::SetUnitType(EUnitType Type)
{
	UnitType = Type;
}

void AGhostUnitSpawning::Spawn()
{
	switch (UnitType)
	{
	case EUnitType::U_Tank:
		{
			// Spawn Tank
			Mesh->SetStaticMesh( MeshUnits[0]);
			break;
		}
	case EUnitType::U_Warrior:
		{
			// Spawn Warrior
			Mesh->SetStaticMesh( MeshUnits[1]);
			break;
		}
	case EUnitType::U_Mage:
		{
			// Spawn Mage
			Mesh->SetStaticMesh( MeshUnits[2]);
			Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
			break;
		}
	case EUnitType::U_Leader:
		{
			// Spawn Leader
			Mesh->SetStaticMesh( MeshUnits[3]);
			Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
			break;
		}
	default:
		{
			// Spawn Default
			Mesh->SetStaticMesh( MeshUnits[0]);
			Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
			break;
		}
	}
}


