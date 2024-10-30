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

	SetReplicates(true);

	
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
	DOREPLIFETIME(AProjectile, FireBallSys_NS);
	DOREPLIFETIME(AProjectile, FireBallComp_NS);
}
	
	// ----------------------------
	// Projectile LifeSpan



void AProjectile::Server_CreateProjectile_Implementation()
{
	Multi_CreateProjectile();
}

void AProjectile::Multi_CreateProjectile_Implementation()
{
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
			FireBallComp_NS->Activate();
		}
	}
}

void AProjectile::CalculateProjectileDirection(AActor* Target)
{
	if (!IsMageAttack){
		float XValue = UKismetMathLibrary::Abs(TowerOwner->GetGridPosition().X - TowerOwner->Grid->ConvertLocationToIndex(Target->GetActorLocation()).X);
		float YValue = UKismetMathLibrary::Abs(TowerOwner->GetGridPosition().Y - TowerOwner->Grid->ConvertLocationToIndex(Target->GetActorLocation()).Y);
		float ZValue = TowerOwner->GetActorLocation().Z / 50 - Target->GetActorLocation().Z / 50;

		float NewTimeToTarget = 0.35;

		if (XValue != 0 && YValue != 0 && XValue > YValue)	
			NewTimeToTarget += FMath::Sqrt(UKismetMathLibrary::Abs((XValue * XValue) - (YValue * YValue)) * 0.17) + UKismetMathLibrary::Abs(0.5 * ZValue);
		else if (XValue != 0 && YValue != 0 && XValue <= YValue)
			NewTimeToTarget += FMath::Sqrt(UKismetMathLibrary::Abs((YValue * YValue) - (XValue * XValue)) * 0.17) + UKismetMathLibrary::Abs(0.5 * ZValue);

		GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Emerald, FString::SanitizeFloat(NewTimeToTarget));
	
		UGameplayStatics::SuggestProjectileVelocity_MovingTarget(
			GetWorld(),
			MoveVelocity,
			GetActorLocation(),
			Target,
			FVector(0,0,25),
			0,
			NewTimeToTarget);
	}
	else
	{
		float XValue = UKismetMathLibrary::Abs(UnitOwner->GetIndexPosition().X - UnitOwner->Grid->ConvertLocationToIndex(Target->GetActorLocation()).X);
		float YValue = UKismetMathLibrary::Abs(UnitOwner->GetIndexPosition().Y - UnitOwner->Grid->ConvertLocationToIndex(Target->GetActorLocation()).Y);
		float ZValue = UnitOwner->GetActorLocation().Z / 50 - Target->GetActorLocation().Z / 50;

		float NewTimeToTarget = 2.35;

		if (XValue != 0 && YValue != 0 && XValue > YValue)	
			NewTimeToTarget += FMath::Sqrt(UKismetMathLibrary::Abs((XValue * XValue) - (YValue * YValue)) * 0.17) + UKismetMathLibrary::Abs(0.5 * ZValue);
		else if (XValue != 0 && YValue != 0 && XValue <= YValue)
			NewTimeToTarget += FMath::Sqrt(UKismetMathLibrary::Abs((YValue * YValue) - (XValue * XValue)) * 0.17) + UKismetMathLibrary::Abs(0.5 * ZValue);

		GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Emerald, FString::SanitizeFloat(NewTimeToTarget));
	
		UGameplayStatics::SuggestProjectileVelocity_MovingTarget(
			GetWorld(),
			MoveVelocity,
			GetActorLocation(),
			Target,
			FVector(0,0,25),
			0,
			NewTimeToTarget);
	}
}

void AProjectile::MoveProjectile(AActor* Actor)
{
	GetWorld()->GetTimerManager().SetTimer(
		CannonBallTimer,
		this,
		&AProjectile::DestroyProjectile,
		1.25f,
		false);

	FString DebugMessage = FString::Printf(TEXT("%lld %lld %lld"), FMath::RoundToInt(MoveVelocity.X), FMath::RoundToInt(MoveVelocity.Y), FMath::RoundToInt(MoveVelocity.Z));
	GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Emerald, DebugMessage);
	
	CalculateProjectileDirection(Actor);
	ProjectileMovement->BeginReplication();
	ProjectileMovement->Velocity = MoveVelocity;
	
	DebugMessage = FString::Printf(TEXT("%lld %lld %lld"), FMath::RoundToInt(MoveVelocity.X), FMath::RoundToInt(MoveVelocity.Y), FMath::RoundToInt(MoveVelocity.Z));
	GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Emerald, DebugMessage);
}

void AProjectile::DestroyProjectile()
{
	if (TowerOwner)
		TowerOwner->CannonBall = nullptr;
	if (UnitOwner)
		UnitOwner->FireBall = nullptr;
	
	//Destroy(true, true);
}

