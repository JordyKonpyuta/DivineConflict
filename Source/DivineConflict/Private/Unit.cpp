// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit.h"
#include "Grid.h"
#include "GridInfo.h"


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
	return IInteractInterface::Interact_Implementation(PlayerController);
}

// Called when the game starts or when spawned
void AUnit::BeginPlay()
{
	Super::BeginPlay();

	if(Grid != nullptr)
	{
		Grid->GridInfo->AddUnitInGrid(Grid->ConvertLocationToIndex(GetActorLocation()), this);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Unit on tile %s and this position is %s"), *GetName(), *IndexPosition.ToString()));
	}
	
}

void AUnit::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
	SetIsSelected(true);
	DisplayWidget();
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

FVector2d AUnit::GetIndexPosition()
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

void AUnit::SetIndexPosition(FVector2D ip)
{
	IndexPosition = ip;
}

void AUnit::DisplayWidget_Implementation()
{
}

