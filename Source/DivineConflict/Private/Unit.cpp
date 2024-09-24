// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit.h"
#include "CameraPlayer.h"
#include "CustomPlayerController.h"
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

void AUnit::Move()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Unit move"));
	
	if(PlayerControllerRef != nullptr)
	{
		Path.Empty();
		Path = PlayerControllerRef->CameraPlayerRef->Path;
	}
	if (Path.Num()!=-1)
	{
		for(FIntPoint index : Path)
		{
			FVector location = Grid->ConvertIndexToLocation(index);
			Grid->GridVisual->RemoveStateFromTile(index, EDC_TileState::Pathfinding);
			SetActorLocation(location);
		}
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Unit moved"));
		
		Grid->GridInfo->setUnitIndexOnGrid(Grid->ConvertLocationToIndex(GetActorLocation()),this);
		if(Grid->GetGridData()->Find(Grid->ConvertLocationToIndex(GetActorLocation()))->TileType == EDC_TileType::Gate)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Unit is on gate"));

			//layerControllerRef->DisplayWidgetEndGame();
			
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Unit is not on gate"));
		}
	}
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

UTexture2D* AUnit::GetUnitIcon()
{
	return UnitIcon;
}

FIntPoint AUnit::GetIndexPosition()
{
	return IndexPosition;
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

void AUnit::SetUnitIcon(UTexture2D* i)
{
	UnitIcon = i;
}

void AUnit::SetIndexPosition(FIntPoint ip)
{
	IndexPosition = ip;
}


