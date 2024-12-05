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
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	RootComponent = Mesh;
	Arrow->SetupAttachment(Mesh);
	Arrow->SetVisibility(true);
	Arrow->SetHiddenInGame(false);

	SetReplicates(true);
}

// Replicated properties
void ABase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ABase, AllSpawnLoc);
	DOREPLIFETIME(ABase, GridPosition);
	DOREPLIFETIME(ABase, Health);
	DOREPLIFETIME(ABase, Level);
	DOREPLIFETIME(ABase, MaxHealth);
	DOREPLIFETIME(ABase, PlayerOwner);
	DOREPLIFETIME(ABase, GoldCostUpgrade);
	DOREPLIFETIME(ABase, StoneCostUpgrade);
	DOREPLIFETIME(ABase, WoodCostUpgrade);
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
		GetWorld()->GetTimerManager().SetTimer( UnusedHandle, this, &ABase::TimerBeginPlay, 2.f, false );
		// Grid : Building
		TimerBeginPlay();
		// Grid : Spawnable Locations
		int Ratio = 1;


		if (GetActorRotation().Yaw == 0)
		{
			AllSpawnLoc.AddUnique(GetGridPosition() + FIntPoint(1*Ratio, 0));
			AllSpawnLoc.AddUnique(GetGridPosition() + FIntPoint(0, 1*Ratio));
			AllSpawnLoc.AddUnique(GetGridPosition() + FIntPoint(1*Ratio, -1*Ratio));
			AllSpawnLoc.AddUnique(GetGridPosition() + FIntPoint(-1*Ratio, 1*Ratio));
		}
		else if (GetActorRotation().Yaw > 80 && GetActorRotation().Yaw < 100)
		{
			AllSpawnLoc.AddUnique(GetGridPosition() + FIntPoint(0, 1*Ratio));
			AllSpawnLoc.AddUnique(GetGridPosition() + FIntPoint(-1*Ratio, 0));
			AllSpawnLoc.AddUnique(GetGridPosition() + FIntPoint(1*Ratio, 1*Ratio));
			AllSpawnLoc.AddUnique(GetGridPosition() + FIntPoint(-1*Ratio, -1*Ratio));
		}
		else if ((GetActorRotation().Yaw > 175 && GetActorRotation().Yaw < 185) || (GetActorRotation().Yaw > -185 && GetActorRotation().Yaw < -175))
		{
			AllSpawnLoc.AddUnique(GetGridPosition() + FIntPoint(-1*Ratio, 0));
			AllSpawnLoc.AddUnique(GetGridPosition() + FIntPoint(0, -1*Ratio));
			AllSpawnLoc.AddUnique(GetGridPosition() + FIntPoint(-1*Ratio, 1*Ratio));
			AllSpawnLoc.AddUnique(GetGridPosition() + FIntPoint(1*Ratio, -1*Ratio));
		}
		else if (GetActorRotation().Yaw > -100 && GetActorRotation().Yaw < -80)
		{
			AllSpawnLoc.AddUnique(GetGridPosition() + FIntPoint(0, -1*Ratio));
			AllSpawnLoc.AddUnique(GetGridPosition() + FIntPoint(1*Ratio, 0));
			AllSpawnLoc.AddUnique(GetGridPosition() + FIntPoint(-1*Ratio, -1*Ratio));
			AllSpawnLoc.AddUnique(GetGridPosition() + FIntPoint(1*Ratio, 1*Ratio));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("*ERROR*")));
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Rotation : %f"), GetActorRotation().Yaw));;
		}
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

void ABase::TimerBeginPlay()
{
	Grid->GridInfo->AddBaseOnGrid(Grid->ConvertLocationToIndex(GetActorLocation()), this);
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
	for (FIntPoint i : AllSpawnLoc)
	{
		if (IsSelected)
			{
				if (Grid->GetGridData()->Find(i)->UnitOnTile == nullptr)
				{
					Grid->GridVisual->AddStateToTile(i, EDC_TileState::Spawnable);
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

void ABase::Server_Upgrade_Implementation()
{
	PlayerStateRef->SetMaxUnits(PlayerStateRef->GetMaxUnits() + 5);
}


void ABase::Multi_Upgrade_Implementation()
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
			if (Level < MaxLevel)
			{
				UE_LOG( LogTemp, Warning, TEXT("0!!!"));
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("0!!!")));
				PlayerStateRef->SetMaxUnits(PlayerStateRef->GetMaxUnits() + 5);
				Server_Upgrade();
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("MaxUnitCount = %d"), PlayerStateRef->GetMaxUnits()));
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

float ABase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator,
	AActor* DamageCauser)
{
	Health -= DamageAmount;
	ServerCheckIfDead(Health);
	
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
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
		GetWorld()->GetGameState<ACustomGameState>()->ServerVictoryScreen(PlayerOwner);
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
	for (FIntPoint i : AllSpawnLoc)
	{
		if (Grid->GetGridData()->Find(i)->TileState.Contains(EDC_TileState::Spawned))
		{
			count++;
		}
	}

	if (count == AllSpawnLoc.Num())
	{
		CanSpawn = false;
	}
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