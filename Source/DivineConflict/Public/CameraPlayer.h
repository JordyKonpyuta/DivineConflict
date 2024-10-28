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

	// UPROPERTIES
public:
	// ----------------------------
	// Components
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivate))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivate))
	UCameraComponent* FollowCamera;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	USceneComponent* CameraRoot;
	
	// ----------------------------
	// References
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivate))
	ACustomPlayerController* CustomPlayerController;

	// ----------------------------
	// Input Actions
	
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
	
	// ----------------------------
	// Timers
	
	FTimerHandle RepeatMoveTimer;
	
	// ----------------------------
	// Delegates
	
	FTimerDelegate RepeatMoveTimerDelegate;

	// ----------------------------
	// Actions

	// MOVEMENT

	UPROPERTY()
	int UnitMovingCurrentMovNumber;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variable", meta = (AllowPrivate = "true"))
	TArray<FIntPoint> Path;
	
	UPROPERTY()
	FVector FullMoveDirection;

	// RESTRICTIONS
	UPROPERTY(VisibleAnywhere , BlueprintReadWrite, Category = "Variable", meta = (AllowPrivate = "true"))
	bool IsMovingUnit = false;
	
	UPROPERTY(VisibleAnywhere , BlueprintReadWrite, Category = "Variable", meta = (AllowPrivate = "true"))
	bool IsAttacking = false;

	UPROPERTY(VisibleAnywhere , BlueprintReadWrite, Category = "Variable", meta = (AllowPrivate = "true"))
	bool IsTowering = false;

	UPROPERTY(VisibleAnywhere , BlueprintReadWrite, Category = "Variable", meta = (AllowPrivate = "true"))
	bool IsSpelling = false;

	UPROPERTY(VisibleAnywhere , BlueprintReadWrite, Category = "Variable", meta = (AllowPrivate = "true"))
	bool IsSpawningUnit = false;

	
	// ----------------------------
	// Camera Movement
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Variable", meta = (AllowPrivate = "true"))
	float ZoomCameraSpeed = 15.0f;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Variable", meta = (AllowPrivate = "true"))
	FRotator SnapRotation;
	
protected:

	// ----------------------------
	// Controls
	
	UPROPERTY()
	FInputActionValue ValueInput = FInputActionValue();
	
	// ----------------------------
	// Action - Movement

	UPROPERTY()
	FVector OldMoveDirection;
	
	// ----------------------------
	// Camera's Rotation
	
	FRotator TargetRotationPitch=FRotator( -20, 0 ,0 );
	FRotator TargetRotationYaw=FRotator( 0, 10 ,0 );

	
	// UFUNCTIONS
public:
	// ----------------------------
	// Constructor

	ACameraPlayer();

	// ----------------------------
	// Override
	
	virtual void Tick(float DeltaTime) override;

	// ----------------------------
	// Camera Movement

	// TRUE MOVEMENT
	UFUNCTION()
	void UpdatedMove(const FInputActionValue& Value);
	
	UFUNCTION()
	void MoveCamera(/* const FInputActionValue& Value*/);

	void PathRemove(const FInputActionValue& Value);

	void PathClear();

	// TIMER
	UFUNCTION()
	void RepeatMoveTimerCamera(const FInputActionValue& Value);
	UFUNCTION()
	void StopRepeatMoveTimerCamera();
	
	// ----------------------------
	// Camera Rotation

	void RotateCamera(const FInputActionValue& Value);

	void RotateCameraPitch(const FInputActionValue& Value);
	
	// ----------------------------
	// Camera Zoom

	void ZoomCamera( const FInputActionValue& Value);
	
	// ----------------------------
	// Widget

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget")
	void RotateWidget(float ValueX, float ValueY);
	
	// ----------------------------
	// Setter
	
	void SetCustomPlayerController(ACustomPlayerController* Cpc);

	//void FreeCam();

protected:
	// ----------------------------
	// Override
	
	virtual void BeginPlay() override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// ----------------------------
	// Interactions

	void Interaction();
};
