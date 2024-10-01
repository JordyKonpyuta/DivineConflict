// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"

#include "BuildingSpawnLocation.h"
#include "CustomGameState.h"
#include "CustomPlayerController.h"
#include "Grid.h"
#include "GridInfo.h"
#include "Unit_Child_Leader.h"
#include "Unit_Child_Warrior.h"
#include "Unit_Child_Mage.h"
#include "Unit_Child_Tank.h"

// Sets default values
ABuilding::ABuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Add and name a static mesh
	StaticMeshBuilding = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building Appearance"));

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComp;

	StaticMeshBuilding->SetCollisionResponseToAllChannels(ECR_Ignore);

	StaticMeshPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane StaticMesh"));
    StaticMeshPlane->SetHiddenInGame(true);
	StaticMeshPlane->SetupAttachment(RootComponent);
	StaticMeshBuilding->SetupAttachment(RootComponent);

	AllMaterials.Add(ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("/Script/Engine.Material'/Game/Core/Texture_DEBUG/M_NeutralPlayer.M_NeutralPlayer'")).Object);
	AllMaterials.Add(ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Core/Texture_DEBUG/Mi_HeavenPlayer.Mi_HeavenPlayer'")).Object);
	AllMaterials.Add(ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Core/Texture_DEBUG/Mi_HellPlayer.Mi_HellPlayer'")).Object);
	

}

bool ABuilding::Interact_Implementation(ACustomPlayerController* PlayerController)
{
	PlayerControllerRef = PlayerController;
	return true;
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

	if (Grid)
	{
		// Create Building Data
		
		if(BuildingList == EBuildingList::B_AP)
		{
// Get Position Stocked
			SpawnLocRef.Add(Grid->ConvertLocationToIndex(GetActorLocation()));
			SpawnLocRef.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(100, 100, 0)));
			SpawnLocRef.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(100, 0, 0)));
			SpawnLocRef.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(100, -100, 0)));
			SpawnLocRef.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(0, 100, 0)));
			SpawnLocRef.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(0, -100, 0)));
			SpawnLocRef.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-100, 100, 0)));
			SpawnLocRef.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-100, 0, 0)));
			SpawnLocRef.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-100, -100, 0)));
			
			// Grid : Building
			Grid->GridInfo->addBuildingOnGrid(SpawnLocRef[0], this);
			Grid->GridInfo->addBuildingOnGrid(SpawnLocRef[1], this);
			Grid->GridInfo->addBuildingOnGrid(SpawnLocRef[2], this);
			Grid->GridInfo->addBuildingOnGrid(SpawnLocRef[3], this);
			Grid->GridInfo->addBuildingOnGrid(SpawnLocRef[4], this);
			Grid->GridInfo->addBuildingOnGrid(SpawnLocRef[5], this);
			Grid->GridInfo->addBuildingOnGrid(SpawnLocRef[6], this);
			Grid->GridInfo->addBuildingOnGrid(SpawnLocRef[7], this);
			Grid->GridInfo->addBuildingOnGrid(SpawnLocRef[8], this);

			UnitPosition = GetActorLocation();
		}
		else
		{
			// Grid : Building
			UnitPosition = GetActorLocation();
			
			//Get Position Stocked
			SpawnLocRef.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(50, 50, 0)));
			SpawnLocRef.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(50, -50, 0)));
			SpawnLocRef.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-50, 50, 0)));
			SpawnLocRef.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-50, -50, 0)));

			// Add itself to the Grid
			Grid->GridInfo->addBuildingOnGrid(SpawnLocRef[0], this);
			Grid->GridInfo->addBuildingOnGrid(SpawnLocRef[1], this);
			Grid->GridInfo->addBuildingOnGrid(SpawnLocRef[2], this);
			Grid->GridInfo->addBuildingOnGrid(SpawnLocRef[3], this);

			// Grid : Spawners
			AllSpawnLoc.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(150, 50, 0)));
			AllSpawnLoc.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(150, -50, 0)));
			AllSpawnLoc.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(50, 150, 0)));
			AllSpawnLoc.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(50, -150, 0)));
			AllSpawnLoc.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-50, 150, 0)));
			AllSpawnLoc.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-50, -150, 0)));
			AllSpawnLoc.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-150, 50, 0)));
			AllSpawnLoc.Add(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-150, -50, 0)));
			
			BuildingSpawnLocationRef->GridRef = Grid;
		}
		
		switch (BuildingList)
		{
		case EBuildingList::B_Gold:
			UnitRef = GetWorld()->SpawnActor<AUnit_Child_Warrior>(Grid->ConvertIndexToLocation(SpawnLocRef[0]), FRotator(0, 0, 0));
			break;
		case EBuildingList::B_Wood:
			UnitRef = GetWorld()->SpawnActor<AUnit_Child_Mage>(Grid->ConvertIndexToLocation(SpawnLocRef[0]), FRotator(0, 0, 0));
			break;
		case EBuildingList::B_Stone:
			UnitRef = GetWorld()->SpawnActor<AUnit_Child_Tank>(Grid->ConvertIndexToLocation(SpawnLocRef[0]), FRotator(0, 0, 0));
			break;
		case EBuildingList::B_AP:
			UnitRef = GetWorld()->SpawnActor<AUnit_Child_Leader>(Grid->ConvertIndexToLocation(SpawnLocRef[0]), FRotator(0, 0, 0));
			break;
		default:
			UnitRef = GetWorld()->SpawnActor<AUnit_Child_Warrior>(Grid->ConvertIndexToLocation(SpawnLocRef[0]), FRotator(0, 0, 0));
		}
		GarrisonFull = true;
		UnitRef->SetIsGarrison(true);
		UnitRef->SetBuildingRef(this);
		UnitRef->SetActorLocation(GetActorLocation());
		StaticMeshBuilding->SetMaterial(0, AllMaterials[0]);

		GameStateRef = Cast<ACustomGameState>(GetWorld()->GetGameState());
	
		GameStateRef->OnTurnSwitchDelegate.AddUniqueDynamic(this,
			&ABuilding::OnTurnChanged);
	}
}

void ABuilding::SwitchOwner(ACustomPlayerState* NewOwner)
{
	switch (BuildingList)
	{
	case EBuildingList::B_Wood:
		if (OwnerPlayerState){OwnerPlayerState->WoodBuildingOwned -= 1;}
		NewOwner->WoodBuildingOwned += 1;
		break;
	case EBuildingList::B_Stone:
		if (OwnerPlayerState){OwnerPlayerState->StoneBuildingOwned -= 1;}
		NewOwner->StoneBuildingOwned += 1;
		break;
	case EBuildingList::B_Gold:
		if (OwnerPlayerState){OwnerPlayerState->GoldBuildingOwned -= 1;}
		NewOwner->GoldBuildingOwned += 1;
		break;
	case EBuildingList::B_AP:
		if (OwnerPlayerState){OwnerPlayerState->GotCentralBuilding = false;}
		NewOwner->GotCentralBuilding = true;
		break;
	default:
		break;
	}
	
	PlayerOwner = NewOwner->PlayerTeam;
	
	switch (PlayerOwner)
	{
	case EPlayer::P_Hell:
	StaticMeshBuilding->SetMaterial(0, AllMaterials[2]);
		break;
	case EPlayer::P_Heaven:
	StaticMeshBuilding->SetMaterial(0, AllMaterials[1]);
		break;
	case EPlayer::P_Neutral:
	StaticMeshBuilding->SetMaterial(0, AllMaterials[0]);
		break;
	}
	OwnerPlayerState = NewOwner;
}

// Check if Player is currently passive; will be used to spawn the HUD
bool ABuilding::IsPlayerPassive(ACustomPlayerController* PlayerController)
{
	return PlayerController->GetIsInActiveTurn();
}

// Make a unit spawn in a specific point
void ABuilding::SpawnUnitFromBuilding_Implementation(FIntPoint SpawnLocation)
{
	AUnit* UnitToSpawn = nullptr;
	switch (UnitProduced)
	{
	case EUnitType::U_Warrior:
		UnitToSpawn = GetWorld()->SpawnActor<AUnit_Child_Warrior>(Grid->ConvertIndexToLocation(SpawnLocation), FRotator(0, 0, 0));
		break;
	case EUnitType::U_Mage:
		UnitToSpawn =GetWorld()->SpawnActor<AUnit_Child_Mage>(Grid->ConvertIndexToLocation(SpawnLocation), FRotator(0, 0, 0));
		break;
	case EUnitType::U_Tank:
		UnitToSpawn =GetWorld()->SpawnActor<AUnit_Child_Tank>(Grid->ConvertIndexToLocation(SpawnLocation), FRotator(0, 0, 0));
		break;
	default:
		UnitToSpawn =GetWorld()->SpawnActor<AUnit_Child_Warrior>(Grid->ConvertIndexToLocation(SpawnLocation), FRotator(0, 0, 0));
	}
	UnitToSpawn->SetUnitTeam(PlayerOwner);
}

void ABuilding::removeUnitRef()
{
	UnitRef = nullptr;
	GarrisonFull = false;
}

void ABuilding::OnTurnChanged()
{
	if (UnitRef && OwnerPlayerState)
	{
		if (OwnerPlayerState->bIsActiveTurn == false)
		{
			if (UnitRef->GetCurrentHealth() + 3 > UnitRef->GetMaxHealth())
			{
				UnitRef->SetCurrentHealth(UnitRef->GetMaxHealth());
			}
			else
			{
				UnitRef->SetCurrentHealth(UnitRef->GetCurrentHealth() + 3);
			}
		}
	}
}


// Called every frame
void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FIntPoint ABuilding::GetGridPosition()
{
	return GridPosition;
}

void ABuilding::SetGridPosition(FIntPoint GridP)
{
	GridPosition = GridP;
}

EBuildingList ABuilding::GetBuildingList()
{
	return BuildingList;
}
