// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"

#include "BuildingSpawnLocation.h"
#include "CustomGameState.h"
#include "CustomPlayerController.h"
#include "Grid.h"
#include "GridInfo.h"
#include "GridPath.h"
#include "TutorialGameMode.h"
#include "Unit_Child_Leader.h"
#include "Unit_Child_Warrior.h"
#include "Unit_Child_Mage.h"
#include "Unit_Child_Tank.h"
#include "Net/UnrealNetwork.h"

// ----------------------------
	// Initialisation
ABuilding::ABuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
	
	// Add and name a static mesh
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComp;
	
	StaticMeshBuilding = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Building Appearance"));
	StaticMeshBuilding->SetupAttachment(RootComponent);
	StaticMeshBuilding->SetIsReplicated(true);

	StaticMeshBuilding->SetCollisionResponseToAllChannels(ECR_Ignore);

	StaticMeshPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane StaticMesh"));
    StaticMeshPlane->SetHiddenInGame(true);
	StaticMeshPlane->SetupAttachment(RootComponent);

	AllMaterials.Add(ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("/Script/Engine.Material'/Game/Core/Texture_DEBUG/M_NeutralPlayer.M_NeutralPlayer'")).Object);
	AllMaterials.Add(ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Core/Texture_DEBUG/Mi_HeavenPlayer.Mi_HeavenPlayer'")).Object);
	AllMaterials.Add(ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Core/Texture_DEBUG/Mi_HellPlayer.Mi_HellPlayer'")).Object);
	

}


// ----------------------------
	// Overrides
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
	case EBuildingList::B_Smol:
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

			// Unit
			UnitPosition = GetActorLocation();
		}
		else if (BuildingList == EBuildingList::B_Smol)
		{
			// Get Position Stocked
			SpawnLocRef.Add(Grid->ConvertLocationToIndex(GetActorLocation()));
			
			// Grid : Building
			Grid->GridInfo->addBuildingOnGrid(SpawnLocRef[0], this);

			// Unit
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
		
		if(HasAuthority())
		{
			switch (BuildingList)
			{
			case EBuildingList::B_Gold:
				UnitRef = GetWorld()->SpawnActor<AUnit_Child_Warrior>(Grid->ConvertIndexToLocation(SpawnLocRef[0]), FRotator(0, 0, 0));
				UnitRef->SetPlayerOwner(PlayerOwner);
				break;
			case EBuildingList::B_Wood:
				UnitRef = GetWorld()->SpawnActor<AUnit_Child_Mage>(Grid->ConvertIndexToLocation(SpawnLocRef[0]), FRotator(0, 0, 0));
				UnitRef->SetPlayerOwner(PlayerOwner);
				break;
			case EBuildingList::B_Stone:
				UnitRef = GetWorld()->SpawnActor<AUnit_Child_Tank>(Grid->ConvertIndexToLocation(SpawnLocRef[0]), FRotator(0, 0, 0));
				UnitRef->SetPlayerOwner(PlayerOwner);
				break;
			case EBuildingList::B_AP:
				UnitRef = GetWorld()->SpawnActor<AUnit_Child_Leader>(Grid->ConvertIndexToLocation(SpawnLocRef[0]), FRotator(0, 0, 0));
				UnitRef->SetPlayerOwner(PlayerOwner);
				break;
			case EBuildingList::B_Smol:
				break;
			default:
				UnitRef = GetWorld()->SpawnActor<AUnit_Child_Warrior>(Grid->ConvertIndexToLocation(SpawnLocRef[0]), FRotator(0, 0, 0));
				UnitRef->SetPlayerOwner(PlayerOwner);
			}
			Multi_InitStartBuilding(UnitRef);

		}

		
		StaticMeshBuilding->SetMaterial(0, AllMaterials[0]);
		
		GameStateRef = Cast<ACustomGameState>(GetWorld()->GetGameState());
	
		GameStateRef->OnTurnSwitchDelegate.AddUniqueDynamic(this,
			&ABuilding::OnTurnChanged);
	}
}

// Called every frame
void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABuilding::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABuilding, GarrisonFull);
}

void ABuilding::Multi_InitStartBuilding_Implementation(AUnit* UnitSp)
{
	if (UnitSp)
	{
		GarrisonFull = true;
		UnitSp->SetIsGarrison(true);
		UnitSp->SetBuildingRef(this);
		UnitSp->SetActorLocation(GetActorLocation());
	}
}


bool ABuilding::Interact_Implementation(ACustomPlayerController* PlayerController)
{
	PlayerControllerRef = PlayerController;
	PrepareMovements();
	return true;
}

	// ----------------------------
	// Prepare Actions
void ABuilding::BuildingPreAction(AUnit* UnitSp)
{
	if(UnitSp)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BuildingPreAction"));
		UnitSpawned = UnitSp;
		UnitSpawned->GetFinalGhostMesh()->SetVisibility(true);
		UnitSpawned->GetStaticMesh()->SetVisibility(false);
		bHasSpawned = true;
	}
}

	// ----------------------------
	// Actions

void ABuilding::BuildingAction()
{
	if(UnitSpawned)
	{
		UnitSpawned->GetFinalGhostMesh()->SetVisibility(false);
		UnitSpawned->GetStaticMesh()->SetVisibility(true);
		UnitSpawned = nullptr;
		bHasSpawned = false;
	}
}

	// ----------------------------
	// Prepare Movement

void ABuilding::PrepareMovements()
{
	MovementOptionsReady = true;
	// Tiles you can walk on after leaving the building
	if (!SpawnLocRef.IsEmpty())
	{
		for (FIntPoint CurrentBuildingTile : SpawnLocRef)
		{
			MovementOptions.AddUnique(CurrentBuildingTile);
			for (FIntPoint CurrentTileChecked : Grid->GridPath->NewFindPath(CurrentBuildingTile, PlayerControllerRef))
			{
				MovementOptions.AddUnique(CurrentTileChecked);
			}
		}
	}
}

	// ----------------------------
	// Handling Turns
// Check if Player is currently passive; will be used to spawn the HUD
bool ABuilding::IsPlayerPassive(ACustomPlayerController* PlayerController)
{
	return PlayerController->GetIsInActiveTurn();
}

void ABuilding::OnTurnChanged()
{
	if (UnitRef && OwnerPlayerState)
	{
		if (OwnerPlayerState->bIsActiveTurn == false)
		{
			if (UnitRef->GetCurrentHealth() + 2 > UnitRef->GetMaxHealth())
			{
				UnitRef->SetCurrentHealth(UnitRef->GetMaxHealth());
			}
			else
			{
				UnitRef->SetCurrentHealth(UnitRef->GetCurrentHealth() + 2);
			}
		}
	}
}

	// ----------------------------
	// GETTERS
// Grid Position
FIntPoint ABuilding::GetGridPosition()
{
	return GridPosition;
}

// Type
EBuildingList ABuilding::GetBuildingList()
{
	return BuildingList;
}

	// ----------------------------
	// SETTERS
// Grid Position
void ABuilding::SetGridPosition(FIntPoint GridP)
{
	GridPosition = GridP;
}

// Owner & Type
void ABuilding::SwitchOwner(ACustomPlayerState* NewOwner)
{
	Server_SwitchOwner(NewOwner);
}

void ABuilding::Server_SwitchOwner_Implementation(ACustomPlayerState* NewOwner)
{
	Multi_SwitchOwner(NewOwner);
}

void ABuilding::Multi_SwitchOwner_Implementation(ACustomPlayerState* NewOwner)
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
	case EBuildingList::B_Smol:
		if (OwnerPlayerState){OwnerPlayerState->SmolBuildingOwned -= 1;}
		NewOwner->SmolBuildingOwned += 1;
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
		if (NewOwner->bIsInTutorial == true && GarrisonFull)
			GetWorld()->GetAuthGameMode<ATutorialGameMode>()->DisplayTutorialWidget(7);
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
