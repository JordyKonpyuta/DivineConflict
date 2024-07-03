// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Base.generated.h"

UCLASS()
class DIVINECONFLICT_API ABase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Stats
	UPROPERTY()
	int Health;

	UPROPERTY()
	int Attack;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Getter
	int GetHealth();
	int GetAttack();

	// Setter
	void SetHealth(int h);
	void SetAttack(int a);

};
