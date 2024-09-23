// Fill out your copyright notice in the Description page of Project Settings.


#include "Base.h"
#include "CustomPlayerController.h"
#include "Grid.h"
#include "GridInfo.h"
#include "CustomPlayerState.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABase::ABase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABase::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ACustomPlayerController::StaticClass(),FoundActor);
	for(AActor* Actor : FoundActor)
	{
		PlayerControllerRef = Cast<ACustomPlayerController>(Actor);
	}

	PlayerStateRef = PlayerControllerRef->GetPlayerState<ACustomPlayerState>();

	if (Grid)
	{
		// Create Building Data
	
		// Grid : Building
		Grid->GridInfo->addBaseOnGrid(Grid->ConvertLocationToIndex(GetActorLocation()), this);
		Grid->GridInfo->addBaseOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(100, -100, 0)), this);
		Grid->GridInfo->addBaseOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(100, 0, 0)), this);
		Grid->GridInfo->addBaseOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(100, 100, 0)), this);
		Grid->GridInfo->addBaseOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(0, 100, 0)), this);
		Grid->GridInfo->addBaseOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(0, -100, 0)), this);
		Grid->GridInfo->addBaseOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-100, -100, 0)), this);
		Grid->GridInfo->addBaseOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-100, 0, 0)), this);
		Grid->GridInfo->addBaseOnGrid(Grid->ConvertLocationToIndex(GetActorLocation() + FVector3d(-100, 100, 0)), this);
		}
}

// Called every frame
void ABase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int ABase::GetHealth()
{
	return Health;
}

int ABase::GetAttack()
{
	return Attack;
}

bool ABase::GetIsHell()
{
	return IsHell;
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

void ABase::SetAttack(int a)
{
	Attack = a;
}

void ABase::SetIsHell(bool bH)
{
	IsHell = bH;
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
		if (PlayerControllerRef->GetIsHell() == IsHell)
		{
			PlayerControllerRef->SetIsDead(true);
			OnDeath();
			
		}
	}
}

void ABase::OnInteract_Implementation()
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

