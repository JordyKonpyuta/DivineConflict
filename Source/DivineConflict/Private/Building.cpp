// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"


// Sets default values
ABuilding::ABuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Add and name a static mesh
	StaticMeshBuilding = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building Appearance"));
	RootComponent = StaticMeshBuilding;
}

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();

	switch (BuildingList) {
	case EBuildingList::B_Wood:
		UnitProduced = EUnitType::U_Mage;
		break;
	case EBuildingList::B_Stone:
		UnitProduced = EUnitType::U_Tank;
		break;
	case EBuildingList::B_Gold:
		UnitProduced = EUnitType::U_Warrior;
		break;
	case EBuildingList::B_AP:
		break;
	default:
		break;
	}
}

void ABuilding::SpawnUnit()
{
	
}

void ABuilding::SwitchOwner(ACustomPlayerState* NewOwner)
{
	switch (BuildingList)
	{
	case EBuildingList::B_Wood:
		OwnerPlayerState->WoodBuildingOwned -= 1;
		NewOwner->WoodBuildingOwned += 1;
		break;
	case EBuildingList::B_Stone:
		OwnerPlayerState->StoneBuildingOwned -= 1;
		NewOwner->StoneBuildingOwned += 1;
		break;
	case EBuildingList::B_Gold:
		OwnerPlayerState->GoldBuildingOwned -= 1;
		NewOwner->GoldBuildingOwned += 1;
		break;
	case EBuildingList::B_AP:
		OwnerPlayerState->GotCentralBuilding = false;
		NewOwner->GotCentralBuilding = true;
		break;
	default:
		break;
	}
	
	switch (PlayerOwner) {
	case EPlayer::P_Hell:
		break;
	case EPlayer::P_Heaven:
		break;
	case EPlayer::P_Neutral:
		break;
	default:
		break;
	}
	OwnerPlayerState = NewOwner;
}

// Called every frame
void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

