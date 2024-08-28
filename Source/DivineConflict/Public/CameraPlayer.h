// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CameraPlayer.generated.h"

class ACustomPlayerController;

UCLASS()

class DIVINECONFLICT_API ACameraPlayer : public APawn
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivate))
	ACustomPlayerController* CustomPlayerController;
	

	

	// Sets default values for this pawn's properties
	ACameraPlayer();

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void setCustomePlayerController(ACustomPlayerController* cpc);

	void MoveCamera();

	void RotateCamera();

	void ZoomCamera(float Value);

	//void FreeCam();

};
