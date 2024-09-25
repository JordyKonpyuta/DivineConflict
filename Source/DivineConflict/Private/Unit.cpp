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
	UnitMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	
	
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

void AUnit::Move(TArray<FIntPoint> PathIn)
{

	Path = PathIn;
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
	}
}

void AUnit::AttackUnit(AUnit* UnitToAttack)
{
	if(UnitToAttack == nullptr || Grid == nullptr)
	{
		return;
	}
	SetCurrentHealth(GetCurrentHealth()-( UnitToAttack->GetAttack()-GetDefense()) );
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("health Attack: ") + FString::FromInt(GetCurrentHealth()));
	UnitToAttack->SetCurrentHealth( UnitToAttack->GetCurrentHealth() - (GetAttack()- UnitToAttack->GetDefense()));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("health Defence: ") + FString::FromInt(UnitToAttack->GetCurrentHealth()));
	

	if(UnitToAttack->GetCurrentHealth() < 1)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Unit defend is dead"));
		Path.Add(UnitToAttack->GetIndexPosition());
		Grid->GridInfo->RemoveUnitInGrid(UnitToAttack);
		GetWorld()->DestroyActor(UnitToAttack);
		if(GetCurrentHealth() < 1)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Unit attack is dead"));
			Grid->GridInfo->RemoveUnitInGrid(this);
			Grid->GridVisual->RemoveStateFromTile(IndexPosition, EDC_TileState::Selected);
			GetWorld()->DestroyActor(this);
		}
		else
		Move(Path);
	}
	if(GetCurrentHealth() < 1)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Unit attack is dead"));
		Grid->GridInfo->RemoveUnitInGrid(this);
		Grid->GridVisual->RemoveStateFromTile(IndexPosition, EDC_TileState::Selected);
		GetWorld()->DestroyActor(this);
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


