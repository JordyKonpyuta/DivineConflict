// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CustomPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class DIVINECONFLICT_API ACustomPlayerState : public APlayerState
{
	GENERATED_BODY()

	// UPROPERTIES //
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actions")
	bool GotCentralBuilding = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actions")
	int MaxActionPoints = 10 + (GotCentralBuilding * 5);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actions")
	int ActionPoints = MaxActionPoints;

private:


	// UFUNCTIONS //
public:
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void RefreshActionPoints();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	int GetActionPoints();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	int GetMaxActionPoints();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool IsCentralBuildingOurs();

private:

};
