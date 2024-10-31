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

	// Get PlayerState
	GetWorld()->GetTimerManager().SetTimer(
		PlayerStateHandle,
		this,
		&ABase::AssignPlayerState,
		0.5,
		true );
}
	
	// ----------------------------
	// Timer

void ABase::timerBeginPlay()
{
	Grid->GridInfo->addBaseOnGrid(Grid->ConvertLocationToIndex(GetActorLocation()), this);
}

void ABase::AssignPlayerState()
{
	if (!PlayerStateRef)
	{
		SetPlayerState();
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(PlayerStateHandle);
	}
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
		if (IsSelected)
			{
				if (Grid->GetGridData()->Find(i)->UnitOnTile == nullptr)
				{
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
	UE_LOG( LogTemp, Warning, TEXT("3"));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("3")));
	SetPlayerState();
	if(PlayerStateRef){
		// Check if player has enough resources
		UE_LOG( LogTemp, Warning, TEXT("2"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("2")));
		if (PlayerStateRef->GetWoodPoints() >= WoodCostUpgrade && PlayerStateRef->GetStonePoints() >= StoneCostUpgrade && PlayerStateRef->GetGoldPoints() >= GoldCostUpgrade)
		{
			UE_LOG( LogTemp, Warning, TEXT("1"));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("1")));
			if (PlayerStateRef->GetMaxUnits() < 15 && Level < MaxLevel)
			{
				UE_LOG( LogTemp, Warning, TEXT("0!!!"));
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("0!!!")));
				PlayerStateRef->SetMaxUnits(PlayerStateRef->GetMaxUnits() + 5);
				PlayerStateRef->ChangeWoodPoints(WoodCostUpgrade, false);
				PlayerStateRef->ChangeStonePoints(StoneCostUpgrade, false);
				PlayerStateRef->ChangeGoldPoints(GoldCostUpgrade, false);
				SetCostsUpgrade(GoldCostUpgrade += 10, StoneCostUpgrade += 10, WoodCostUpgrade += 10);
				Level++;
			}
		}
	}
}

	// ----------------------------
	// Take Damage

void ABase::BaseTakeDamage(int Damage)
{
	Health -= Damage;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Base Health : %d"), Health));
	ServerCheckIfDead(Health);
}

	// ----------------------------
	// Check Death

void ABase::ServerCheckIfDead_Implementation(int H)
{
	MulticastCheckIfDead(H);
}

void ABase::MulticastCheckIfDead_Implementation(int H)
{
	if (H <= 0)
	{
		if(!PlayerStateRef)
			SetPlayerState();
		
		if(PlayerStateRef)
		{
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
	// Tiles Check

bool ABase::CheckTiles()
{
	int count = 0;
	CanSpawn = true;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("CheckTiles")));
	for (FIntPoint i : AllSpawnLoc)
	{
		if (Grid->GetGridData()->Find(i)->TileState.Contains(EDC_TileState::Spawned))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Tile Spawned")));
			count++;
		}
	}

	if (count == 4)
	{
		CanSpawn = false;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("CanSpawn : %d"), CanSpawn));
	return CanSpawn;
	

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