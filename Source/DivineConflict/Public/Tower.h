// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tower.generated.h"

class AUnit;

UCLASS()
class DIVINECONFLICT_API ATower : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATower();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* Mesh;

	UFUNCTION(BlueprintNativeEvent)
	void SetMesh();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	AUnit* UnitInGarnison = nullptr;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	int Attack = 10;
	UPROPERTY()
	bool IsHell = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	int GetAttack();
	UFUNCTION(BlueprintCallable)
	void SetAttack(int NewAttack);
	UFUNCTION(BlueprintCallable)
	bool GetIsHell();
	UFUNCTION(BlueprintCallable)
	void SetIsHell(bool NewIsHell);

	UFUNCTION(BlueprintCallable)
	AUnit* GetUnitInGarnison();
	UFUNCTION(BlueprintCallable)
	void SetUnitInGarnison(AUnit* NewUnitInGarnison);

};
