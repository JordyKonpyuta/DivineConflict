// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ClimbActor.generated.h"

UCLASS()
class DIVINECONFLICT_API AClimbActor : public AActor
{
	GENERATED_BODY()

	// UPROPERTIES
public:	
	// ----------------------------
	// Constructor
	AClimbActor();
	
	// ----------------------------
	// Components
	
	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Appearance")
	TObjectPtr<UStaticMeshComponent> StaticMeshClimbable;
	

protected:

	// UFUNCTIONS
public:	
	// ----------------------------
	// Override
	
	virtual void Tick(float DeltaTime) override;

protected:
	// ----------------------------
	// Override

	virtual void BeginPlay() override;
	
};
