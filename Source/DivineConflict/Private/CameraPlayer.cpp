// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPlayer.h"

// Sets default values
ACameraPlayer::ACameraPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACameraPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACameraPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACameraPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACameraPlayer::setCustomePlayerController(ACustomPlayerController* cpc)
{
	CustomPlayerController = cpc;
}

void ACameraPlayer::MoveCamera()
{
	//UEngine::AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Move Camera"));
}

void ACameraPlayer::RotateCamera()
{
	//UEngine::AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Rotate Camera"));
}

void ACameraPlayer::ZoomCamera(float Value)
{
	//UEngine::AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Zoom Camera"));	
}

