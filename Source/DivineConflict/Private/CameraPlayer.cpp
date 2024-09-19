// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPlayer.h"
#include "CustomPlayerController.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Grid.h"
#include "GridPath.h"
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
	CameraBoom->SetRelativeRotation( FRotator(-60, 0, 0));
	
	
	
	
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

	if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(AIMove, ETriggerEvent::Started, this, &ACameraPlayer::MoveCamera);
		EnhancedInputComponent->BindAction(AIRotate, ETriggerEvent::Started, this, &ACameraPlayer::RotateCamera);
		EnhancedInputComponent->BindAction(AIZoom, ETriggerEvent::Triggered, this, &ACameraPlayer::ZoomCamera);
		EnhancedInputComponent->BindAction(AIInteraction,ETriggerEvent::Started, this, &ACameraPlayer::Interaction);
	}

}

void ACameraPlayer::Interaction()
{
	CustomPlayerController->ControllerInteration();
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
			MoveDirection = UKismetMathLibrary::GetForwardVector(UKismetMathLibrary::MakeRotator(0,0,GetActorRotation().Yaw)) * (Input.X*100);

		}
		else
		{
			 MoveDirection = CameraBoom->GetRightVector() * (Input.Y*100);
		}
		
		if (IsMovingUnit)
		{
			//print IsMovingUnit
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("IsMovingUnit"));
			
			
			if (Path.Num()< CustomPlayerController->UnitRef->GetPM())
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Path : ") + FString::FromInt(Path.Num()));

				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("X : ") + FString::FromInt(CustomPlayerController->Grid->ConvertLocationToIndex(MoveDirection).X));
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Y : ") + FString::FromInt(CustomPlayerController->Grid->ConvertLocationToIndex(MoveDirection).Y));
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Path : ") + FString::FromInt(CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(MoveDirection))->TileTransform.GetLocation().Z));
				
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Path : ") + FString::FromInt(CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(GetActorLocation()))->TileTransform.GetLocation().Z));
				if (CustomPlayerController->Grid->GridPath->IsValidHeigh(CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(MoveDirection)),
					CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(GetActorLocation()))))
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Path : ") + FString::FromInt(Path.Num()+1));
				
					
					if (CustomPlayerController->Grid->IsTileWalkable(CustomPlayerController->Grid->ConvertLocationToIndex(MoveDirection)))
					{
						
						Path.AddUnique(CustomPlayerController->Grid->ConvertLocationToIndex(GetActorLocation()));

						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Path : ") + FString::FromInt(Path.Num()+2));
				
						this->SetActorLocation(this->GetActorLocation() + MoveDirection);
					}
				}
			}
			
		}
		else
		{
			this->SetActorLocation(this->GetActorLocation() + MoveDirection);
		}
		

	}
	
}

void ACameraPlayer::RotateCamera(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Rotate Camera rotation: ") + FString::SanitizeFloat(CameraBoom->GetComponentRotation().Roll));

	float Input = Value.Get<float>();

	if(abs(Input) > 0.6)
	{
		
		
		SnapRotation = FRotator( CameraBoom->GetComponentRotation().Pitch,CameraBoom->GetComponentRotation().Yaw + UKismetMathLibrary::SignOfFloat(Input) * -90 , 0);

		
		
		
		CameraBoom->SetRelativeRotation(SnapRotation);

		//CameraBoom->AddLocalRotation(FRotator(0, Input*90, 0));
		
	}
	
	
	
}

void ACameraPlayer::ZoomCamera( const FInputActionValue& Value)
{
	const float Input = Value.Get<float>();

		if (Controller != nullptr)
		{
			CameraBoom->TargetArmLength = FMath::Clamp((CameraBoom->TargetArmLength+Input*ZoomCameraSpeed), 750.0f, 3750.0f);
			
		
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Zoom Camera Length: ") + FString::SanitizeFloat(CameraBoom->TargetArmLength));
		}
}

