// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Tower.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Unit_Child_Mage.h"


	// ----------------------------
	// Constructor

AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	
	// Cannonball
	
	CannonBallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	CannonBallMesh->SetStaticMesh( ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'")).Object);
	CannonBallMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	CannonBallMesh->SetIsReplicated(true);
	CannonBallMesh->SetWorldScale3D(FVector(0.25,0.25,0.25));
	RootComponent = CannonBallMesh; // Set as the root component

	//CannonBallMesh->SetSimulatePhysics(true);

	
	// FireBall
	
	FireBallComp_NS = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	FireBallSys_NS = ConstructorHelpers::FObjectFinder<UNiagaraSystem>(TEXT("NiagaraSystem'/Game/Core/FX/NS_Fireball.NS_Fireball'")).Object;
	

	// Movement
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("FloatingPawnMovement"));
	ProjectileVelocity = 400.f;
}
	
	// ----------------------------
	// Overrides

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProjectile, CannonBallMesh);
	DOREPLIFETIME(AProjectile, MoveVelocity);
	DOREPLIFETIME(AProjectile, ProjectileMovement);
	DOREPLIFETIME(AProjectile, ProjectileVelocity);
	DOREPLIFETIME(AProjectile, IsMageAttack);
	DOREPLIFETIME(AProjectile, TowerOwner);
	DOREPLIFETIME(AProjectile, UnitOwner);
	DOREPLIFETIME(AProjectile, FireBallSys_NS);
	DOREPLIFETIME(AProjectile, TimeToTarget);
}
	
	// ----------------------------
	// Projectile LifeSpan



void AProjectile::Server_CreateProjectile_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FireBallComp_NS.GetName());
	Multi_CreateProjectile();
}

void AProjectile::Multi_CreateProjectile_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FireBallComp_NS.GetName());
	if (IsMageAttack)
	{
		FireBallComp_NS = UNiagaraFunctionLibrary::SpawnSystemAttached(
			FireBallSys_NS,
			CannonBallMesh,
			NAME_None,
			FVector(0.f),
			FRotator(0.f),
			EAttachLocation::Type::KeepRelativeOffset,
			false,
			true);

		CannonBallMesh->SetVisibility(false);
		
		if (FireBallComp_NS)
		{
			FireBallComp_NS->SetFloatParameter("SphereRadius", 10);
			FireBallComp_NS->Activate();
		}
	}
}

void AProjectile::CalculateProjectileDirection(AActor* Target)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FireBallComp_NS.GetName());
	if (!IsMageAttack){
		if (TowerOwner)
		{
			const float XValue = UKismetMathLibrary::Abs(TowerOwner->GetGridPosition().X - TowerOwner->Grid->ConvertLocationToIndex(Target->GetActorLocation()).X);
			const float YValue = UKismetMathLibrary::Abs(TowerOwner->GetGridPosition().Y - TowerOwner->Grid->ConvertLocationToIndex(Target->GetActorLocation()).Y);
			const float ZValue = TowerOwner->GetActorLocation().Z / 50 - Target->GetActorLocation().Z / 50;

			TimeToTarget = 0.35;

			if (XValue != 0 && YValue != 0 && XValue > YValue)	
				TimeToTarget += FMath::Sqrt(UKismetMathLibrary::Abs((XValue * XValue) - (YValue * YValue)) * 0.17) + UKismetMathLibrary::Abs(0.5 * ZValue);
			else if (XValue != 0 && YValue != 0 && XValue <= YValue)
				TimeToTarget += FMath::Sqrt(UKismetMathLibrary::Abs((YValue * YValue) - (XValue * XValue)) * 0.17) + UKismetMathLibrary::Abs(0.5 * ZValue);

			UGameplayStatics::SuggestProjectileVelocity_MovingTarget(
				GetWorld(),
				MoveVelocity,
				GetActorLocation(),
				Target,
				FVector(0,0,25),
				0,
				TimeToTarget);
		}
	}
	else
	{
		if (UnitOwner){
			const float XValue = UKismetMathLibrary::Abs(UnitOwner->GetIndexPosition().X - UnitOwner->Grid->ConvertLocationToIndex(Target->GetActorLocation()).X);
			const float YValue = UKismetMathLibrary::Abs(UnitOwner->GetIndexPosition().Y - UnitOwner->Grid->ConvertLocationToIndex(Target->GetActorLocation()).Y);
			const float ZValue = UnitOwner->GetActorLocation().Z / 50 - Target->GetActorLocation().Z / 50;

			TimeToTarget = 0.75;

			if (XValue != 0 && YValue != 0 && XValue > YValue)	
				TimeToTarget += FMath::Sqrt(UKismetMathLibrary::Abs((XValue * XValue) - (YValue * YValue)) * 0.17) + UKismetMathLibrary::Abs(0.5 * ZValue);
			else if (XValue != 0 && YValue != 0 && XValue <= YValue)
				TimeToTarget += FMath::Sqrt(UKismetMathLibrary::Abs((YValue * YValue) - (XValue * XValue)) * 0.17) + UKismetMathLibrary::Abs(0.5 * ZValue);
		
			UGameplayStatics::SuggestProjectileVelocity_MovingTarget(
				GetWorld(),
				MoveVelocity,
				GetActorLocation(),
				Target,
				FVector(0,0,100),
				0,
				TimeToTarget);
		
		}
	}
}

void AProjectile::MoveProjectile(AActor* Actor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FireBallComp_NS.GetName());
	Server_MoveProjectile(Actor);
}

void AProjectile::Server_MoveProjectile_Implementation(AActor* Actor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FireBallComp_NS.GetName());
	Multi_MoveProjectile(Actor);

	GetWorld()->GetTimerManager().SetTimer(
		CannonBallTimer,
		this,
		&AProjectile::Server_RemoveProjectile,
		TimeToTarget,
		false);
}


void AProjectile::Multi_MoveProjectile_Implementation(AActor* Actor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FireBallComp_NS.GetName());
	CalculateProjectileDirection(Actor);
	
	ProjectileMovement->BeginReplication();
	FireBallComp_NS->BeginReplication();
	ProjectileMovement->Velocity = MoveVelocity;
}

void AProjectile::Server_RemoveProjectile_Implementation()
{
	Multi_RemoveProjectile();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FireBallComp_NS.GetName());
	
	GetWorld()->GetTimerManager().SetTimer(
		CannonBallTimer2,
		this,
		&AProjectile::Server_DestroyProjectile,
		0.75f,
		false);
}


void AProjectile::Multi_RemoveProjectile_Implementation()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FireBallComp_NS.GetName());
	
	if (TowerOwner)
		TowerOwner->CannonBall = nullptr;
	if (UnitOwner)
		UnitOwner->FireBall = nullptr;
	
	if (FireBallComp_NS)
	{
		ProjectileMovement->Velocity = FVector(0,0,0);
		ProjectileMovement->bSimulationEnabled = false;
		
		FireBallComp_NS->SetFloatParameter("SphereRadius", 250);
		FireBallComp_NS->Activate();
	}
}

void AProjectile::Server_DestroyProjectile_Implementation()
{
	Multi_DestroyProjectile();
}

void AProjectile::Multi_DestroyProjectile_Implementation()
{
	GetWorldTimerManager().ClearTimer(CannonBallTimer);
	GetWorldTimerManager().ClearTimer(CannonBallTimer2);
	Destroy();
}