// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ClimbActor.generated.h"

UCLASS()
class DIVINECONFLICT_API AClimbActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AClimbActor();

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Appearance")
	UStaticMeshComponent* StaticMeshClimbable;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
