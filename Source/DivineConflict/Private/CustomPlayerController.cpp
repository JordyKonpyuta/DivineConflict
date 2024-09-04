// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Engine\LocalPlayer.h"
#include "CameraPlayer.h"


void ACustomPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CameraPlayerRef = Cast<ACameraPlayer>(GetPawn());

	CameraPlayerRef->setCustomePlayerController(this);

	

	
}

void ACustomPlayerController::SetupInputComponent()
{
	//super::SetupInputComponent();

	if (APlayerController* PlayerController = Cast<APlayerController>(this))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
	
		/*
		//EnhancedInputComponent->BindAction(AIZoom, ETriggerEvent::Triggered, CameraPlayerRef, &ACameraPlayer::ZoomCamera);
		
		EnhancedInputComponent->BindAction(AIMove, ETriggerEvent::Triggered, CameraPlayerRef, &ACameraPlayer::MoveCamera);
		
		EnhancedInputComponent->BindAction(AIRotate, ETriggerEvent::Triggered, CameraPlayerRef, &ACameraPlayer::RotateCamera);
		
		//EnhancedInputComponent->BindAction(AIFreeCam, ETriggerEvent::Triggered, CameraPlayerRef, &ACameraPlayer::FreeCam);
		*/
	
}

void ACustomPlayerController::Move(/*const FInputActionValue& Value*/)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Move"));

	UE_LOG(LogTemp, Warning, TEXT("Move"));

	
	
}


bool ACustomPlayerController::GetIsHell()
{
	return IsHell;
}

void ACustomPlayerController::SetIsHell(bool h)
{
	IsHell = h;
}

bool ACustomPlayerController::GetIsInActiveTurn()
{
	return IsInActiveTurn;
}

void ACustomPlayerController::SetIsInActiveTurn(bool a)
{
	IsInActiveTurn = a;
}

bool ACustomPlayerController::GetIsReady()
{
	return IsReady;
}

void ACustomPlayerController::SetIsReady(bool r)
{
	IsReady = r;
}
