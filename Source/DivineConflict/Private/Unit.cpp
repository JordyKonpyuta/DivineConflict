// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit.h"

#include "Base.h"
#include "Building.h"
#include "CameraPlayer.h"
#include "CustomPlayerController.h"
#include "CustomPlayerState.h"
#include "Grid.h"
#include "GridInfo.h"
#include "GridVisual.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AUnit::AUnit()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UnitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Unit Mesh"));
	SetRootComponent(UnitMesh);
	UnitMesh->SetStaticMesh( ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Game_Art/Asset_temp/Character/Paradis/tank_ange_pose.tank_ange_pose'")).Object);
	UnitMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	UnitName = EUnitName::Tank;

	
	
}

bool AUnit::Interact_Implementation(ACustomPlayerController* PlayerController)
{

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Interact unit"));
	PlayerControllerRef = PlayerController;
	//PlayerControllerRef->CameraPlayerRef->IsMovingUnit = true;
	//DisplayWidget();
	

	
	return true;
}


void AUnit::interaction(ACustomPlayerController* PlayerController)
{
}

void AUnit::SetGrid(AGrid* NewGrid)
{
	Grid = NewGrid;
}

// Called when the game starts or when spawned
void AUnit::BeginPlay()
{
	Super::BeginPlay();

	if(Grid != nullptr)
	{
		if (Grid->GridInfo == nullptr)
		{
			return;
		}

		Grid->GridInfo->AddUnitInGrid(Grid->ConvertLocationToIndex(GetActorLocation()), this);
		
	}
	else 
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Grid is null"));
		SetGrid();
    }
	
}

void AUnit::SetGrid()
{
	//get the grid
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Set Grid"));
	AActor* FoundActors = UGameplayStatics::GetActorOfClass(GetWorld(), AGrid::StaticClass());
	if (FoundActors != nullptr)
	{
		Grid = Cast<AGrid>(FoundActors);
		Grid->GridInfo->AddUnitInGrid(Grid->ConvertLocationToIndex(GetActorLocation()), this);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Grid Found"));
		//delay 0.2s and try again
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AUnit::SetGrid, 0.2f, false);
		
	}
}

void AUnit::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
	SetIsSelected(true);
}

// Called every frame
void AUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AUnit::Move(TArray<FIntPoint> PathIn)
{
	Path.Empty();
	Path = PathIn;
	if (Path.Num()!=-1)
	{
		for(FIntPoint index : Path)
		{
			if (index == Path.Last() && Grid->GetGridData()->Find(index)->UnitOnTile)
			{
				Grid->GridVisual->RemoveStateFromTile(index, EDC_TileState::Pathfinding);
				break;
			}
			FVector location = Grid->ConvertIndexToLocation(index);
			Grid->GridVisual->RemoveStateFromTile(index, EDC_TileState::Pathfinding);
			SetActorLocation(location);

		}
		
		if (Grid->GetGridData()->Find(Path.Last())->BuildingOnTile != nullptr)
		{
			SetActorLocation(Grid->GetGridData()->Find(Path.Last())->BuildingOnTile->GetActorLocation());
		}
		
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Unit moved"));
		
		Grid->GridInfo->setUnitIndexOnGrid(Grid->ConvertLocationToIndex(GetActorLocation()),this);
	}
}

void AUnit::AttackUnit(AUnit* UnitToAttack)
{
	if(UnitToAttack == nullptr || Grid == nullptr || UnitToAttack == this)
	{
		return;
	}
	if (UnitToAttack->GetBuffTank())
	{
		if(UnitToAttack->GetAttack()-(GetDefense()+1)  > 0)
			SetCurrentHealth(GetCurrentHealth()-( UnitToAttack->GetAttack()-(GetDefense()+1 )));
	}
	else
	{
		if(UnitToAttack->GetAttack()- GetDefense() > 0)
			SetCurrentHealth( GetCurrentHealth() - (UnitToAttack->GetAttack()- GetDefense()));
	}
	if(GetBuffTank())
	{
		if(GetAttack()- UnitToAttack->GetDefense() > 0)
			UnitToAttack->SetCurrentHealth( UnitToAttack->GetCurrentHealth() - (GetAttack()- UnitToAttack->GetDefense()));
	}
	else 
	{
		if(GetAttack()- UnitToAttack->GetDefense() > 0)
			UnitToAttack->SetCurrentHealth( UnitToAttack->GetCurrentHealth() - (GetAttack()- UnitToAttack->GetDefense()));
	}

	

	if(UnitToAttack->GetCurrentHealth() < 1)
	{
		Path.Add(UnitToAttack->GetIndexPosition());
		Grid->GridInfo->RemoveUnitInGrid(UnitToAttack);
		PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->SetUnits(PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->GetUnits() - 1);
		GetWorld()->DestroyActor(UnitToAttack);
		if(GetCurrentHealth() < 1)
		{

			Grid->GridInfo->RemoveUnitInGrid(this);
			Grid->GridVisual->RemoveStateFromTile(IndexPosition, EDC_TileState::Selected);
			PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->SetUnits(PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->GetUnits() - 1);
			GetWorld()->DestroyActor(this);
		}
		else
		Move(Path);
	}
	if(GetCurrentHealth() < 1)
	{
		Grid->GridInfo->RemoveUnitInGrid(this);
		Grid->GridVisual->RemoveStateFromTile(IndexPosition, EDC_TileState::Selected);
		PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->SetUnits(PlayerControllerRef->GetPlayerState<ACustomPlayerState>()->GetUnits() - 1);
		GetWorld()->DestroyActor(this);
	}


}

void AUnit::AttackBase(ABase* BaseToAttack)
{
	if(BaseToAttack == nullptr || Grid == nullptr)
	{
		return;
	}
	BaseToAttack->TakeDamage(/*GetAttack()*/100);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("health Base: ") + FString::FromInt(BaseToAttack->GetHealth()));
}

void AUnit::Special()
{
	
}

void AUnit::SpecialUnit(AUnit* UnitToAttack)
{
}

void AUnit::SpecialBase(ABase* BaseToAttack)
{
}

void AUnit::NewTurn()
{
	SetIsSelected(false);
	bIsClimbing = false;
	SetBuffTank(false);
	
}

int AUnit::GetAttack()
{
	return Attack;
}

int AUnit::GetDefense()
{
	return Defense;
}

int AUnit::GetCurrentHealth()
{
	return CurrentHealth;
}

int AUnit::GetMaxHealth()
{
	return MaxHealth;
}

int AUnit::GetMovementCost()
{
	return MovementCost;
}

int AUnit::GetAttackCost()
{
	return AttackCost;
}

int AUnit::GetSpellCost()
{
	return SpellCost;
}

bool AUnit::GetIsSelected()
{
	return IsSelected;
}

bool AUnit::GetIsHell()
{
	return IsHell;
}

void AUnit::SetAttack(int a)
{
	Attack = a;
}

void AUnit::SetDefense(int d)
{
	Defense = d;
}

void AUnit::SetCurrentHealth(int ch)
{
	CurrentHealth = ch;
}

void AUnit::SetMaxHealth(int mh)
{
	MaxHealth = mh;
}

void AUnit::SetMovementCost(int mc)
{
	MovementCost = mc;
}

void AUnit::SetAttackCost(int ac)
{
	AttackCost = ac;
}

void AUnit::SetSpellCost(int sc)
{
	SpellCost = sc;
}

void AUnit::SetIsSelected(bool s)
{
	IsSelected = s;
}

void AUnit::SetIsHell(bool h)
{
	IsHell = h;
}

int AUnit::GetTotalDamageInflicted()
{
	return TotalDamagesInflicted;
}

FString AUnit::GetName()
{
	return Name;
}

int AUnit::GetPM()
{
	return PM;
}


FIntPoint AUnit::GetIndexPosition()
{
	return IndexPosition;
}

EPlayer AUnit::GetPlayerOwner()
{
	return PlayerOwner;
}

bool AUnit::GetIsClimbing()
{
	return bIsClimbing;
}

bool AUnit::GetBuffTank()
{
	return bBuffTank;
}

void AUnit::SetTotalDamageInflicted(int tdi)
{
	TotalDamagesInflicted = tdi;
}

void AUnit::SetName(FString n)
{
	Name = n;
}

void AUnit::SetPM(int p)
{
	PM = p;
}


void AUnit::SetIndexPosition(FIntPoint ip)
{
	IndexPosition = ip;
}

void AUnit::SetPlayerOwner(EPlayer po)
{
	PlayerOwner = po;
}

void AUnit::SetIsClimbing(bool ic)
{
	bIsClimbing = ic;
}

void AUnit::SetBuffTank(bool bt)
{
	bBuffTank = bt;
}


