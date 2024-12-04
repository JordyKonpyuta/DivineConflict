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
	// --       Components       --
	// ----------------------------
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivate))
	TObjectPtr<USpringArmComponent> CameraBoom = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivate))
	TObjectPtr<UCameraComponent> FollowCamera = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate = "true"))
	TObjectPtr<USceneComponent> CameraRoot = nullptr;
	
	// ----------------------------
	// --       References       --
	// ----------------------------
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivate))
	TObjectPtr<ACustomPlayerController> CustomPlayerController = nullptr;
	
	// ----------------------------
	// --         Timers         --
	// ----------------------------
	
	FTimerHandle RepeatMoveTimer;
	
	// ----------------------------
	// --        Delegates       --
	// ----------------------------
	
	FTimerDelegate RepeatMoveTimerDelegate;

	// ----------------------------
	// --        Actions         --
	// ----------------------------

	// MOVEMENT

	UPROPERTY()
	int UnitMovingCurrentMovNumber = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variable", meta = (AllowPrivate = "true"))
	TArray<FIntPoint> Path;
	
	UPROPERTY()
	FVector FullMoveDirection = FVector(0, 0, 0);

	UPROPERTY()
	bool FirstMove = true;

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
	// --    Camera Movements    --
	// ----------------------------
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Variable", meta = (AllowPrivate = "true"))
	float ZoomCameraSpeed = 15.0f;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Variable", meta = (AllowPrivate = "true"))
	FRotator SnapRotation;
	
	// ----------------------------
	// --    Camera Rotations    --
	// ----------------------------

	UPROPERTY(Blueprintable, BlueprintReadWrite)
	FRotator TargetRotationPitch=FRotator( -20, 0 ,0 );

	UPROPERTY(Blueprintable, BlueprintReadWrite)
	FRotator TargetRotationYaw=FRotator( 0, 10 ,0 );
	
protected:
	// ----------------------------
	// --        Controls        --
	// ----------------------------
	
	UPROPERTY()
	FInputActionValue ValueInput = FInputActionValue();
	
	// ----------------------------
	// --   Movements - Actions  --
	// ----------------------------

	UPROPERTY()
	FVector OldMoveDirection;
	
	// UFUNCTIONS
public:
	// ----------------------------
	// --       Constructor      --
	// ----------------------------

	ACameraPlayer();
	
	// ----------------------------
	// --        Override        --
	// ----------------------------
	
	virtual void Tick(float DeltaTime) override;
	
	// ----------------------------
	// --     Camera Movement    --
	// ----------------------------

	// TRUE MOVEMENT
	UFUNCTION()
	void UpdatedMove(const FInputActionValue& Value);
	
	UFUNCTION()
	void MoveCamera(/* const FInputActionValue& Value*/);

	UFUNCTION()
	void CancelAction(const FInputActionValue& Value);

	UFUNCTION()
	void PathClear();

	UFUNCTION()
	void ClearMoveMode();

	// TIMER
	UFUNCTION()
	void RepeatMoveTimerCamera(const FInputActionValue& Value);
	
	UFUNCTION()
	void StopRepeatMoveTimerCamera();
	
	// ----------------------------
	// --     Camera Rotation    --
	// ----------------------------

	void RotateCamera(const FInputActionValue& Value);

	void RotateCameraPitch(const FInputActionValue& Value);
	
	// ----------------------------
	// --       Camera Zoom      --
	// ----------------------------

	void ZoomCamera( const FInputActionValue& Value);
	
	// ----------------------------
	// --         Widgets        --
	// ----------------------------

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget")
	void RotateWidget();
	
	// ---------------------------- //
	// --         SETTERS        -- //
	// ---------------------------- //
	
	void SetCustomPlayerController(ACustomPlayerController* Cpc);

protected:
	// ----------------------------
	// --        Override        --
	// ----------------------------
	
	virtual void BeginPlay() override;

};
