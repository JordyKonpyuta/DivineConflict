// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SequenceTuto.generated.h"

class ABase;
class ABuilding;
class ATutorialGameMode;
class ACustomGameState;
class AGrid;
class AUnit;

UCLASS()
class DIVINECONFLICT_API ASequenceTuto : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASequenceTuto();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	int SequenceIndex = 0;

	UPROPERTY()
	bool bSequenceFinished = false;

	UPROPERTY()
	TObjectPtr<AUnit> UnitKilled = nullptr;

	UPROPERTY()
	TObjectPtr<AUnit> UnitKilled2 = nullptr;

	UPROPERTY()
	TObjectPtr<AUnit> UnitPlaying1 = nullptr;

	UPROPERTY()
	TObjectPtr<AUnit> UnitPlaying2 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	TObjectPtr<AGrid> GridRef = nullptr;

	UPROPERTY()
	TObjectPtr<ACustomGameState> GameStateRef = nullptr;

	UPROPERTY()
	TObjectPtr<ATutorialGameMode> TutorialGameModeRef = nullptr;

	UPROPERTY()
	int SpawnUnitSequence = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	TObjectPtr<ABuilding> BuildingSequence = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	TObjectPtr<ABase> BaseSequence = nullptr;

	//Timer
	FTimerHandle StartSequenceTimerHandle;
	


	
public:
	
	UFUNCTION()
	void EndSequence();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void StartSequence();

	UFUNCTION()
	void NextSequence();

	UFUNCTION()
	bool IsSequenceFinished();

	UFUNCTION()
	void Sequence1();

	UFUNCTION()
	void Sequence2();

	UFUNCTION()
	void Sequence3();

	UFUNCTION()
	void Sequence4();

	UFUNCTION()
	void Sequence5();

	UFUNCTION()
	void Sequence6();

	UFUNCTION()
	void Sequence7();

	UFUNCTION()
	void Sequence8();

	UFUNCTION()
	void Sequence9();
	

};
