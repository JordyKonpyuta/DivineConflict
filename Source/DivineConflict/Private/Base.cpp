// Fill out your copyright notice in the Description page of Project Settings.


#include "Base.h"

#include "CustomGameState.h"
#include "CustomPlayerController.h"
#include "Grid.h"
#include "GridInfo.h"
#include "CustomPlayerState.h"
#include "GridVisual.h"
#include "Net/UnrealNetwork.h"
	
	// ----------------------------
	// INITIALISATIONS
// Constructor
ABase::ABase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
}

// Replicated properties
void ABase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABase, Health);
	DOREPLIFETIME(ABase, GoldCostUpgrade);
	DOREPLIFETIME(ABase, StoneCostUpgrade);
	DOREPLIFETIME(ABase, WoodCostUpgrade);
	DOREPLIFETIME(ABase, GridPosition);
	DOREPLIFETIME(ABase, AllSpawnLoc);
	DOREPLIFETIME(ABase, PlayerOwner);
	DOREPLIFETIME(ABase, MaxHealth);
}

// Called when the game starts or when spawned
void ABase::BeginPlay()
{
	Super::BeginPlay();
	FTimerHandle UnusedHandle;
	
	for (APlayerState* CurrentPlayerState : GetWorld()->GetGameState<ACustomGameState>()->PlayerArray)
	{
		if (ACustomPlayerState* CurrentCustomPlayerState = Cast<ACustomPlayerState>(CurrentPlayerState))
        {
            if (CurrentCustomPlayerState->PlayerTeam == PlayerOwner)
            {
                PlayerStateRef = CurrentCustomPlayerState;
            }
        }
	}


	SetMesh();
	// Get Grid
	if (Grid)
	{
		if(HasAuthority())
			GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, TEXT(" Server Base Begin Play"));
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, TEXT(" Client Base Begin Play"));
		}
		GetWorld()->GetTimerManager().SetTimer( UnusedHandle, this, &ABase::timerBeginPlay, 2.f, false );
		// Grid : Building
		timerBeginPlay();
		// Grid : Spawnable Locations
		int Ratio = 0;

		if (PlayerOwner == EPlayer::P_Heaven) Ratio = -1;
		else if (PlayerOwner == EPlayer::P_Hell) Ratio = 1;
	
		AllSpawnLoc.Add(GetGridPosition() + FIntPoint(1*Ratio, 0));
		AllSpawnLoc.Add(GetGridPosition() + FIntPoint(0, 1*Ratio));
		AllSpawnLoc.Add(GetGridPosition() + FIntPoint(1*Ratio, -1*Ratio));
		AllSpawnLoc.Add(GetGridPosition() + FIntPoint(-1*Ratio, 1*Ratio));
	}
	else
	{
		UE_LOG( LogTemp, Warning, TEXT("Grid is NULL"));
	}
	
}

void ABase::timerBeginPlay()
{
	GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Silver, TEXT("addBaseGrid"));
	Grid->GridInfo->addBaseOnGrid(Grid->ConvertLocationToIndex(GetActorLocation()), this);
}


// Called every frame
void ABase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


	
	// ----------------------------
	// Spawn Visuals

void ABase::VisualSpawn()
{
	AllSpawnLoc.Empty();
	int Ratio = 0;

	if (PlayerOwner == EPlayer::P_Heaven) Ratio = -1;
	else if (PlayerOwner == EPlayer::P_Hell) Ratio = 1;
			
	AllSpawnLoc.Add(GetGridPosition() + FIntPoint(1*Ratio, 0));
	AllSpawnLoc.Add(GetGridPosition() + FIntPoint(0, 1*Ratio));
	AllSpawnLoc.Add(GetGridPosition() + FIntPoint(1*Ratio, -1*Ratio));
	AllSpawnLoc.Add(GetGridPosition() + FIntPoint(-1*Ratio, 1*Ratio));
	
	for (FIntPoint i : AllSpawnLoc)
	{
		UE_LOG( LogTemp, Warning, TEXT("i : %s"), *i.ToString());
		if (IsSelected)
			{
			UE_LOG( LogTemp, Warning, TEXT("i : %s"), *i.ToString());
				if (Grid->GetGridData()->Find(i)->UnitOnTile == nullptr)
				{
					UE_LOG( LogTemp, Warning, TEXT("i : %s"), *i.ToString());
					Grid->GridVisual->addStateToTile(i, EDC_TileState::Spawnable);
					return;
				}
			}
		else
		{
			Grid->GridVisual->RemoveStateFromTile(i, EDC_TileState::Spawnable);
		}
	}
}

	// ----------------------------
	// Upgrade

void ABase::Upgrade()
{
	if(PlayerStateRef)
		// Check if player has enough resources
			if (PlayerStateRef->GetWoodPoints() >= WoodCostUpgrade && PlayerStateRef->GetStonePoints() >= StoneCostUpgrade && PlayerStateRef->GetGoldPoints() >= GoldCostUpgrade)
			{
				if (PlayerStateRef->MaxUnitCount < 15 && Level < MaxLevel)
				{
					PlayerStateRef->MaxUnitCount += 5;
					PlayerStateRef->ChangeWoodPoints(WoodCostUpgrade, false);
					PlayerStateRef->ChangeStonePoints(StoneCostUpgrade, false);
					PlayerStateRef->ChangeGoldPoints(GoldCostUpgrade, false);
					SetCostsUpgrade(GoldCostUpgrade += 10, StoneCostUpgrade += 10, WoodCostUpgrade += 10);
					Level++;
				}
			}

			else UE_LOG( LogTemp, Warning, TEXT("Player State Ref is NULL"));
}

	// ----------------------------
	// Prepare Actions

void ABase::BasePreAction(AUnit* UnitSp)
{/*
	if(UnitSp)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BasePreAction"));
		UnitSpawned = UnitSp;
		UnitSpawned->GetFinalGhostMesh()->SetVisibility(true);
		UnitSpawned->GetStaticMesh()->SetVisibility(false);
	}*/
}

	// ----------------------------
	// Actions

void ABase::BaseAction()
{/*
	if(UnitSpawned)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BaseAction"));
		UnitSpawned->GetFinalGhostMesh()->SetVisibility(false);
		UnitSpawned->GetStaticMesh()->SetVisibility(true);
		UnitSpawned = nullptr;
	}*/
}

	// ----------------------------
	// Take Damage

void ABase::TakeDamage(int Damage)
{
	Health -= Damage;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Base Health : %d"), Health));
	ServerCheckIfDead(Health);
}

	// ----------------------------
	// Check Death

void ABase::ServerCheckIfDead_Implementation(int H)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Base Health : %d"), H));
	MulticastCheckIfDead(H);
}

void ABase::MulticastCheckIfDead_Implementation(int H)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Base Health : %d"), H));
	if (H <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Base is Dead"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Base is Dead"));
		if(!PlayerStateRef)
			SetPlayerState();
		
		if(PlayerStateRef)
		{
			UE_LOG( LogTemp, Warning, TEXT("PlayerStateRef is not NULL"));
			if (PlayerStateRef->bIsInTutorial) OnDeath();
			else GetWorld()->GetGameState<ACustomGameState>()->ServerVictoryScreen(PlayerOwner);
		}
	}	
}

	// ----------------------------
	// UI

void ABase::OnDeath_Implementation()
{
}

	// ----------------------------
	// GETTERS

// Stats
int ABase::GetHealth()
{
	return Health;
}

// Costs
int ABase::GetGoldCostUpgrade()
{
	return GoldCostUpgrade;
}

int ABase::GetStoneCostUpgrade()
{
	return StoneCostUpgrade;
}

int ABase::GetWoodCostUpgrade()
{
	return WoodCostUpgrade;
}

// Index Position
FIntPoint ABase::GetGridPosition()
{
	return GridPosition;
}

	// ----------------------------
	// SETTERS

// Components
void ABase::SetMesh_Implementation()
{
}

// Stats
void ABase::SetHealth(int h)
{
	Health = h;
}

// Player State
void ABase::SetPlayerState()
{
	for (APlayerState* CurrentPlayerState : GetWorld()->GetGameState<ACustomGameState>()->PlayerArray)
	{
		if (ACustomPlayerState* CurrentCustomPlayerState = Cast<ACustomPlayerState>(CurrentPlayerState))
		{
			if (CurrentCustomPlayerState->PlayerTeam == PlayerOwner)
			{
				PlayerStateRef = CurrentCustomPlayerState;
			}
		}
	}
}

// Costs
void ABase::SetCostsUpgrade(int g, int s, int w)
{
	GoldCostUpgrade = g;
	StoneCostUpgrade = s;
	WoodCostUpgrade = w;
}

// Index Position
void ABase::SetGridPosition(FIntPoint GridP)
{
	GridPosition = GridP;
}