// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPlayer.h"

#include "Building.h"
#include "CustomGameState.h"
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
	CameraBoom->SetRelativeRotation( FRotator(-25, 10, 0));
	FullMoveDirection = GetActorLocation();
	OldMoveDirection = GetActorLocation();
}

// Called every frame
void ACameraPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

	CameraBoom->SetWorldRotation(FRotator(UKismetMathLibrary::FInterpTo(CameraBoom->GetComponentRotation().Pitch,TargetRotationPitch.Pitch, DeltaTime, 20.0f),
		UKismetMathLibrary::RInterpTo(CameraBoom->GetComponentRotation(),TargetRotationYaw,DeltaTime,10).Yaw,0));

	SetActorLocation(UKismetMathLibrary::VInterpTo(GetActorLocation(),FullMoveDirection,DeltaTime,10));
}

// Called to bind functionality to input
void ACameraPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(AIMove, ETriggerEvent::Started, this, &ACameraPlayer::RepeatMoveTimerCamera);
		EnhancedInputComponent->BindAction(AIMove, ETriggerEvent::Completed, this, &ACameraPlayer::StopRepeatMoveTimerCamera);
		EnhancedInputComponent->BindAction(AIMove,ETriggerEvent::Triggered, this, &ACameraPlayer::UpdatedMove);
		EnhancedInputComponent->BindAction(AIRotate, ETriggerEvent::Started, this, &ACameraPlayer::RotateCamera);
		EnhancedInputComponent->BindAction(AIRotate, ETriggerEvent::Triggered, this, &ACameraPlayer::RotateCameraPitch);
		EnhancedInputComponent->BindAction(AIZoom, ETriggerEvent::Triggered, this, &ACameraPlayer::ZoomCamera);
		EnhancedInputComponent->BindAction(AIInteraction,ETriggerEvent::Started, this, &ACameraPlayer::Interaction);

		EnhancedInputComponent->BindAction(AIRemovePath,ETriggerEvent::Started, this, &ACameraPlayer::PathRemove);
		if(CustomPlayerController)
			EnhancedInputComponent->BindAction(AIEndTurn,ETriggerEvent::Started, CustomPlayerController, &ACustomPlayerController::EndTurn);
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("CustomPlayerController is null"));
			CustomPlayerController = Cast<ACustomPlayerController>(GetController());
			EnhancedInputComponent->BindAction(AIEndTurn,ETriggerEvent::Started, CustomPlayerController, &ACustomPlayerController::EndTurn);
		}
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

void ACameraPlayer::RepeatMoveTimerCamera(const FInputActionValue& Value)
{
	ValueInput = Value;
	MoveCamera();
	RepeatMoveTimerDelegate.BindUFunction(this, "MoveCamera", Value); 
	GetWorld()->GetTimerManager().SetTimer(
		RepeatMoveTimer,
		RepeatMoveTimerDelegate,
		0.1, 
		true,
		0.15); 
}

void ACameraPlayer::UpdatedMove(const FInputActionValue& Value)
{
	ValueInput = Value;
}

void ACameraPlayer::StopRepeatMoveTimerCamera()
{
	GetWorld()->GetTimerManager().ClearTimer(RepeatMoveTimer);
}

void ACameraPlayer::MoveCamera( /*/const FInputActionValue& Value*/)
{

	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("valueinput : ") + FString::FromInt(ValueInput.Get<FVector2d>().X) + " " + FString::FromInt(ValueInput.Get<FVector2d>().Y));
	FVector2d Input = ValueInput.Get<FVector2d>();
	
	if (Controller != nullptr)
	{
		OldMoveDirection = FullMoveDirection;
		FVector3d MoveDirection = FVector3d(0, 0, 0);
		if (abs(Input.X)  >= abs(Input.Y) )
		{
			FVector Forward = FVector(UKismetMathLibrary::Round(CameraBoom->GetForwardVector().X), UKismetMathLibrary::Round(CameraBoom->GetForwardVector().Y), 0);
			MoveDirection = Forward * (UKismetMathLibrary::SignOfFloat(Input.X) * 100);
		}
		else
		{
			FVector Right = FVector(UKismetMathLibrary::Round(CameraBoom->GetRightVector().X), UKismetMathLibrary::Round(CameraBoom->GetRightVector().Y), 0);
			MoveDirection = Right * (UKismetMathLibrary::SignOfFloat(Input.Y) * 100);

		}
		if(!CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection)))
		{
			GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Green, TEXT("Move Camera"));
			return;
		}
		if (IsMovingUnit)
		{
			//print IsMovingUnit
			TArray<FIntPoint> AllReachable = CustomPlayerController->GetPathReachable();

			if (AllReachable.Contains(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection)))
			{
				Path.AddUnique(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection));
				CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection), EDC_TileState::Hovered);
				//this->SetActorLocation(FullMoveDirection);
				FullMoveDirection = OldMoveDirection + MoveDirection;
				CustomPlayerController->Grid->GridVisual->addStateToTile(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection), EDC_TileState::Pathfinding);
			}
			
		}
		else
		{
			CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection), EDC_TileState::Hovered);
			//this->SetActorLocation(FullMoveDirection);
			FullMoveDirection = OldMoveDirection + MoveDirection;
			CustomPlayerController->Grid->GridVisual->addStateToTile(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection), EDC_TileState::Hovered);
		}
	}
}

void ACameraPlayer::RotateCamera(const FInputActionValue& Value)
{
	FVector2d Input = Value.Get<FVector2d>();

	TargetRotationYaw = FRotator(0,TargetRotationYaw.Yaw + UKismetMathLibrary::SignOfFloat(Input.X)*-90, 0);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Input X: ") + FString::SanitizeFloat(Input.X));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Snap Rotation Yaw: ") + FString::SanitizeFloat(CameraBoom->GetComponentRotation().Yaw + UKismetMathLibrary::SignOfFloat(Input.X) * -90));
}

void ACameraPlayer::RotateCameraPitch(const FInputActionValue& Value)
{
	FVector2d Input = Value.Get<FVector2d>();
	
	TargetRotationPitch = FRotator(UKismetMathLibrary::Clamp(TargetRotationPitch.Pitch + Input.Y, -60.0f, -20.0f),0, 0);
}

void ACameraPlayer::ZoomCamera( const FInputActionValue& Value)
{
	const float Input = Value.Get<float>();

		if (Controller != nullptr)
		{
			CameraBoom->TargetArmLength = FMath::Clamp((CameraBoom->TargetArmLength + Input * ZoomCameraSpeed), 750.0f, 2050.0f);
		}
}

void ACameraPlayer::PathRemove(const FInputActionValue& Value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Path Remove"));
	if (Path.Num() == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Path Remove: Empty"));
		return;
	}
	CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(Path.Last(), EDC_TileState::Pathfinding);
	Path.RemoveAt(Path.Num() - 1);

	if(Path.Num() == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Path Remove: Empty!!"));
		for(FIntPoint Point : CustomPlayerController->GetPathReachable())
		{
			CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(Point, EDC_TileState::Reachable);
		}
		CustomPlayerController->SetPlayerAction(EDC_ActionPlayer::None);
		IsMovingUnit = false;
		return;
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Path Remove: " +  Path.Last().ToString()));
	SetActorLocation(FVector( Path.Last().X * 100,Path.Last().Y*100, GetActorLocation().Z));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Path Remove: ") + FString::SanitizeFloat(Path.Num()));
	UE_LOG( LogTemp, Warning, TEXT("Path Remove: %s"), *Path.Last().ToString());
}

void ACameraPlayer::PathClear()
{
	Path.Empty();
}