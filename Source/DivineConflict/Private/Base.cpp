// Fill out your copyright notice in the Description page of Project Settings.


#include "Base.h"
#include "CustomPlayerController.h"
#include "Grid.h"
#include "GridInfo.h"
#include "CustomPlayerState.h"
#include "GridVisual.h"
#include "Unit.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABase::ABase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ABase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABase, Health);
	DOREPLIFETIME(ABase, GoldCostUpgrade);
	DOREPLIFETIME(ABase, StoneCostUpgrade);
	DOREPLIFETIME(ABase, WoodCostUpgrade);
	DOREPLIFETIME(ABase, GridPosition);
	DOREPLIFETIME(ABase, AllSpawnLoc);
}

// Called when the game starts or when spawned
void ABase::BeginPlay()
{
	Super::BeginPlay();

	SetMesh();

	TArray<AActor*> FoundActor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ACustomPlayerController::StaticClass(),FoundActor);
	for(AActor* Actor : FoundActor)
	{
		PlayerControllerRef = Cast<ACustomPlayerController>(Actor);
	}

	PlayerStateRef = PlayerControllerRef->GetPlayerState<ACustomPlayerState>();

	if (Grid)
	{
	
		// Grid : Building
		Grid->GridInfo->addBaseOnGrid(Grid->ConvertLocationToIndex(GetActorLocation()), this);

		// Grid : Spawnable Locations
		int Ratio = 0;

		if (PlayerOwner == EPlayer::P_Heaven) Ratio = -1;
		else if (PlayerOwner == EPlayer::P_Hell) Ratio = 1;
			
		AllSpawnLoc.Add(GetGridPosition() + FIntPoint(1*Ratio, 0));
		AllSpawnLoc.Add(GetGridPosition() + FIntPoint(0, 1*Ratio));
		AllSpawnLoc.Add(GetGridPosition() + FIntPoint(1*Ratio, -1*Ratio));
		AllSpawnLoc.Add(GetGridPosition() + FIntPoint(-1*Ratio, 1*Ratio));
	}
}

// Called every frame
void ABase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABase::VisualSpawn()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Turquoise, TEXT("Update Visual"));
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Turquoise, TEXT("AllSpawnLoc : " + FString::FromInt(AllSpawnLoc.Num())));
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

int ABase::GetHealth()
{
	return Health;
}

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

FIntPoint ABase::GetGridPosition()
{
	return GridPosition;
}

void ABase::SetHealth(int h)
{
	Health = h;
}

void ABase::SetCostsUpgrade(int g, int s, int w)
{
	GoldCostUpgrade = g;
	StoneCostUpgrade = s;
	WoodCostUpgrade = w;
}

void ABase::SetGridPosition(FIntPoint GridP)
{
	GridPosition = GridP;
}

// IF DEAD, END GAME
void ABase::CheckIfDead()
{
	if (Health <= 0)
	{
		Health = 0;
		PlayerControllerRef->SetIsDead(true);
		OnDeath();
		
	}
}

void ABase::SetMesh_Implementation()
{
}


// TAKE DAMAGE
void ABase::TakeDamage(int Damage)
{
	Health -= Damage;
	CheckIfDead();
}

// UPGRADE
void ABase::Upgrade()
{
	if (PlayerStateRef->GetWoodPoints() >= WoodCostUpgrade && PlayerStateRef->GetStonePoints() >= StoneCostUpgrade && PlayerStateRef->GetGoldPoints() >= GoldCostUpgrade)
	{
		if (PlayerStateRef->MaxUnitCount < 15)
		{
			PlayerStateRef->MaxUnitCount += 5;
			SetCostsUpgrade(GoldCostUpgrade += 10, StoneCostUpgrade += 10, WoodCostUpgrade += 10);
		}
	}
}



// FUNCTION FOR UI
void ABase::OnDeath_Implementation()
{
}

