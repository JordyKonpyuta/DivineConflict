// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class AUnit_Child_Mage;
class UNiagaraComponent;
class ATower;
class UFloatingPawnMovement;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;

UCLASS()
class DIVINECONFLICT_API AProjectile : public AActor
{
	GENERATED_BODY()

	// UPROPERTIES
public:	
	// ----------------------------
	// Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> CannonBallMesh;
	
	UPROPERTY(Replicated)
	TObjectPtr<UNiagaraSystem> FireBallSys_NS;
	
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> FireBallComp_NS;
	
	// ----------------------------
	// Projectile Properties

	UPROPERTY(Replicated)
	bool IsMageAttack;

	UPROPERTY(Replicated)
	float TimeToTarget;

	UPROPERTY(Replicated)
	float ProjectileVelocity;

	UPROPERTY(Replicated)
	FVector MoveVelocity;
	
	// ----------------------------
	// Owner

	UPROPERTY(Replicated)
	TObjectPtr<ATower> TowerOwner;

	UPROPERTY(Replicated)
	TObjectPtr<AUnit_Child_Mage> UnitOwner;
	
protected:
	// ----------------------------
	// Components

	UPROPERTY(Replicated)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

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
	FTimerHandle CannonBallTimer2;
	
	UFUNCTION(Server,Reliable)
	void Server_CreateProjectile();
	
	UFUNCTION(NetMulticast,Reliable)
	void Multi_CreateProjectile();
	
	UFUNCTION()
	void CalculateProjectileDirection (AActor* Target);

	UFUNCTION()
	void MoveProjectile(AActor* Actor);

	UFUNCTION(Server, Reliable)
	void Server_MoveProjectile(AActor* Actor);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_MoveProjectile(AActor* Actor);
	
	UFUNCTION(Server, Reliable)
	void Server_RemoveProjectile();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_RemoveProjectile();
	
	UFUNCTION(Server, Reliable)
	void Server_DestroyProjectile();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_DestroyProjectile();
	
protected:
	// ----------------------------
	// Overrides
	
	virtual void BeginPlay() override;
};
