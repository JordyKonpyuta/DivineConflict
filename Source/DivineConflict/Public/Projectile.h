// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class ATower;
class UFloatingPawnMovement;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class DIVINECONFLICT_API AProjectile : public AActor
{
	GENERATED_BODY()

	// UPROPERTIES
public:	
	// ----------------------------
	// Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* CannonBallMesh;
	
	// ----------------------------
	// Projectile Properties

	UPROPERTY(Replicated)
	float ProjectileVelocity;

	UPROPERTY(Replicated)
	FVector MoveVelocity;
	
	// ----------------------------
	// Owner

	UPROPERTY()
	ATower* TowerOwner;
	
protected:
	// ----------------------------
	// Components

	UPROPERTY(Replicated)
	UProjectileMovementComponent* ProjectileMovement;

	// UFUNCTIONS
public:
	// ----------------------------
	// Constructor
	
	AProjectile();
	
	// ----------------------------
	// Overrides
	
	virtual void Tick(float DeltaTime) override;
	
	// ----------------------------
	// Projectile LifeSpan

	FTimerHandle CannonBallTimer;
	
	UFUNCTION()
	void CalculateProjectileDirection (AActor* Target);

	UFUNCTION()
	void MoveProjectile(AActor* Actor);
	
	UFUNCTION()
	void DestroyProjectile();
	
protected:
	// ----------------------------
	// Overrides
	
	virtual void BeginPlay() override;
};
