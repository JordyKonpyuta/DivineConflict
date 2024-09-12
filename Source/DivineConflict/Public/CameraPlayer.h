// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "CameraPlayer.generated.h"


class ACustomPlayerController;
class USpringArmComponent;
class USceneComponent;;
class UInputAction;
class UCameraComponent;

UCLASS()

class DIVINECONFLICT_API ACameraPlayer : public APawn
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivate))
	ACustomPlayerController* CustomPlayerController;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivate))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivate))
	UCameraComponent* FollowCamera;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	UInputAction* AIZoom;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	UInputAction* AIMove;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	UInputAction* AIRotate;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	UInputAction* AIFreeCam;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	UInputAction* AIInteraction;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	UInputAction* AIRemovePath;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	UInputAction* AIEndTurn;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	USceneComponent* CameraRoot;
	
	// Sets default values for this pawn's properties
	ACameraPlayer();
	

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Interaction();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	

	void setCustomePlayerController(ACustomPlayerController* cpc);

	void MoveCamera( const FInputActionValue& Value);

	void RotateCamera(const FInputActionValue& Value);

	void ZoomCamera( const FInputActionValue& Value);

	//void FreeCam();

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Variable", meta = (AllowPrivate = "true"))
	float ZoomCameraSpeed = 15.0f;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Variable", meta = (AllowPrivate = "true"))
	FRotator SnapRotation;

};
