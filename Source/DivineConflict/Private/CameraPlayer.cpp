// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPlayer.h"

#include "Building.h"
#include "CustomPlayerController.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Grid.h"
#include "GridPath.h"
#include "GridVisual.h"
#include "Unit.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ACameraPlayer::ACameraPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraRoot"));
	RootComponent = CameraRoot;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(CameraRoot);
	CameraBoom->TargetArmLength = 750.0f;
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bUsePawnControlRotation = false;
	
	
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	
	

}

// Called when the game starts or when spawned
void ACameraPlayer::BeginPlay()
{
	Super::BeginPlay();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Camera Player Begin Play"));
	CameraBoom->AttachToComponent(CameraRoot, FAttachmentTransformRules::KeepRelativeTransform);
	CameraBoom->SetRelativeRotation( FRotator(-25, 0, 0));
	
	
	
	
}

// Called every frame
void ACameraPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

	CameraBoom->SetWorldRotation(FRotator(UKismetMathLibrary::FInterpTo_Constant(CameraBoom->GetComponentRotation().Pitch, TargetRotationPitch.Pitch, DeltaTime, 10.0f),0,0));


	
}

// Called to bind functionality to input
void ACameraPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(AIMove, ETriggerEvent::Started, this, &ACameraPlayer::MoveCamera);
		EnhancedInputComponent->BindAction(AIRotate, ETriggerEvent::Started, this, &ACameraPlayer::RotateCamera);
		EnhancedInputComponent->BindAction(AIRotate, ETriggerEvent::Triggered, this, &ACameraPlayer::RotateCameraPitch);
		EnhancedInputComponent->BindAction(AIZoom, ETriggerEvent::Triggered, this, &ACameraPlayer::ZoomCamera);
		EnhancedInputComponent->BindAction(AIInteraction,ETriggerEvent::Started, this, &ACameraPlayer::Interaction);
	}
}




void ACameraPlayer::Interaction()
{
	CustomPlayerController->ControllerInteraction();
}


void ACameraPlayer::SetCustomPlayerController(ACustomPlayerController* Cpc)
{
		CustomPlayerController = Cpc;
}

void ACameraPlayer::MoveCamera( const FInputActionValue& Value)
{
	FVector2d Input = Value.Get<FVector2d>();

	if (Controller != nullptr)
	{
		FVector3d MoveDirection = FVector3d(0, 0, 0);
		if (abs(Input.X)  >= abs(Input.Y) )
		{
			MoveDirection = UKismetMathLibrary::GetForwardVector(UKismetMathLibrary::MakeRotator(0,0,GetActorRotation().Yaw)) * (UKismetMathLibrary::SignOfFloat(Input.X) * 100);

		}
		else
		{
			MoveDirection = CameraBoom->GetRightVector() * (UKismetMathLibrary::SignOfFloat(Input.Y)*100);

		}
		if(!CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(GetActorLocation() + MoveDirection)))
			return;

		
		if (IsMovingUnit)
		{
			//print IsMovingUnit
			TArray<FIntPoint> AllReachable = CustomPlayerController->GetPathReachable();

			if (AllReachable.Contains(CustomPlayerController->Grid->ConvertLocationToIndex(this->GetActorLocation() + MoveDirection)))
			{
				Path.AddUnique(CustomPlayerController->Grid->ConvertLocationToIndex(this->GetActorLocation() + MoveDirection));
				CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(CustomPlayerController->Grid->ConvertLocationToIndex(GetActorLocation()), EDC_TileState::Hovered);
				this->SetActorLocation(this->GetActorLocation() + MoveDirection);
				CustomPlayerController->Grid->GridVisual->addStateToTile(CustomPlayerController->Grid->ConvertLocationToIndex(GetActorLocation()), EDC_TileState::Pathfinding);
			}
			
		}
		else
		{
			CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(CustomPlayerController->Grid->ConvertLocationToIndex(GetActorLocation()), EDC_TileState::Hovered);
			this->SetActorLocation(this->GetActorLocation() + MoveDirection);
			CustomPlayerController->Grid->GridVisual->addStateToTile(CustomPlayerController->Grid->ConvertLocationToIndex(GetActorLocation()), EDC_TileState::Hovered);
		}
		

	}
	
}

void ACameraPlayer::RotateCamera(const FInputActionValue& Value)
{
	FVector2d Input = Value.Get<FVector2d>();

	TargetRotationYaw = FRotator(0,CameraBoom->GetComponentRotation().Yaw + UKismetMathLibrary::SignOfFloat(Input.Y) * -90 , 0);
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Snap Rotation Yaw: ") + FString::SanitizeFloat(TargetRotationPitch.Yaw));
}

void ACameraPlayer::RotateCameraPitch(const FInputActionValue& Value)
{
	FVector2d Input = Value.Get<FVector2d>();
	
	TargetRotationPitch = FRotator(UKismetMathLibrary::Clamp((CameraBoom->GetComponentRotation().Pitch + Input.X), -80.0f, -20.0f),TargetRotationPitch.Yaw , 0);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Target Rotation Pitch: ") + FString::SanitizeFloat(TargetRotationPitch.Pitch));
}

void ACameraPlayer::ZoomCamera( const FInputActionValue& Value)
{
	const float Input = Value.Get<float>();

		if (Controller != nullptr)
		{
			CameraBoom->TargetArmLength = FMath::Clamp((CameraBoom->TargetArmLength + Input * ZoomCameraSpeed), 750.0f, 3750.0f);
			
		
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Zoom Camera Length: ") + FString::SanitizeFloat(CameraBoom->TargetArmLength));
		}
}

void ACameraPlayer::PathClear()
{
	Path.Empty();
}

