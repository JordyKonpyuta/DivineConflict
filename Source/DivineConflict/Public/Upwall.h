// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Upwall.generated.h"

UCLASS()
class DIVINECONFLICT_API AUpwall : public AActor
{
	GENERATED_BODY()

	// UPROPERTIES
public:
	
protected:

	// UFUNCTIONS
public:	

	// ----------------------------
	// Constructor
	
	AUpwall();
	
	// ----------------------------
	// Overrides
	
	virtual void Tick(float DeltaTime) override;

protected:
	// ----------------------------
	// Overrides
	
	virtual void BeginPlay() override;
};
