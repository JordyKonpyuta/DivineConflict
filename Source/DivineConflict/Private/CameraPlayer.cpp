// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPlayer.h"
#include "CustomPlayerController.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Grid.h"
#include "GridPath.h"
#include "GridVisual.h"
#include "Unit.h"
#include "Unit_Child_Warrior.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/SpringArmComponent.h"

	// ----------------------------
	// Constructor

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

	// ----------------------------
	// Overrides

void ACameraPlayer::BeginPlay()
{
	Super::BeginPlay();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Camera Player Begin Play"));
	CameraBoom->AttachToComponent(CameraRoot, FAttachmentTransformRules::KeepRelativeTransform);
	CameraBoom->SetRelativeRotation( FRotator(-25, 10, 0));
	FullMoveDirection = GetActorLocation();
	OldMoveDirection = GetActorLocation();
}

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

	// ----------------------------
	// Interactions

void ACameraPlayer::Interaction()
{
	CustomPlayerController->ControllerInteraction();
}

	// ----------------------------
	// Camera Movement - TRUE MOVEMENT

void ACameraPlayer::UpdatedMove(const FInputActionValue& Value)
{
	ValueInput = Value;
}

void ACameraPlayer::MoveCamera( /*/const FInputActionValue& Value*/)
{
	FVector2d Input = ValueInput.Get<FVector2d>();
	
	if (Controller != nullptr)
	{
		OldMoveDirection = FullMoveDirection;
		FVector3d MoveDirection = FVector3d(0, 0, 0);
		if (abs(Input.X)  >= abs(Input.Y) )
		{
			if(CameraBoom->GetComponentRotation().Pitch <= 0 && CameraBoom->GetComponentRotation().Yaw <= 0)
			{
				double Xbase = abs(CameraBoom->GetForwardVector().X); 
				
				double Xmod = 0;
				if (Xbase < 0){Xmod = 1;} else {Xmod = -1;}
				double Ybase = abs(CameraBoom->GetForwardVector().Y);
				double Ymod = 0;
				if (Ybase < 0){Ymod = 1;} else {Ymod = -1;}
				
				if (Xbase > Ybase){Xbase = 1 * Xmod; Ybase = 0;} else {Xbase = 0; Ybase = 1 * Ymod;}
				FVector Forward = FVector(Xbase, Ybase, 0);
				MoveDirection = Forward * (UKismetMathLibrary::SignOfFloat(Input.X) * 100);
			} else
			{
				double Xbase = abs(CameraBoom->GetForwardVector().X);
				double Xmod = 0;
				if (Xbase < 0){Xmod = -1;} else {Xmod = 1;}
				double Ybase = abs(CameraBoom->GetForwardVector().Y);
				double Ymod = 0;
				if (Ybase < 0){Ymod = -1;} else {Ymod = 1;}
				
				if (Xbase > Ybase){Xbase = 1 * Xmod; Ybase = 0;} else {Xbase = 0; Ybase = 1 * Ymod;}
				FVector Forward = FVector(Xbase, Ybase, 0);
				MoveDirection = Forward * (UKismetMathLibrary::SignOfFloat(Input.X) * 100);
			}
		}
		else
		{
			FVector Right = FVector(UKismetMathLibrary::Round(CameraBoom->GetRightVector().X), UKismetMathLibrary::Round(CameraBoom->GetRightVector().Y), 0);
			MoveDirection = Right * (UKismetMathLibrary::SignOfFloat(Input.Y) * 100);

		}
		if(!CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection)))
		{
			return;
		}
		if (IsMovingUnit)
		{
			//print IsMovingUnit
			TArray<FIntPoint> AllReachable = CustomPlayerController->GetPathReachable();
			bool IsWarrior = false;
			if(CustomPlayerController->UnitRef)
				IsWarrior = Cast<AUnit_Child_Warrior>(CustomPlayerController->UnitRef) != nullptr;

			if (!IsWarrior)
			{
				if (AllReachable.Contains(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection))
					&& Path.Num() < CustomPlayerController->UnitRef->GetPM()+1
					&& CustomPlayerController->Grid->GridPath->IsValidHeigh(CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection)),CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection)))
					&& !CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection))->BuildingOnTile
					&& !Path.Contains(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection))
					)
				{
					Path.AddUnique(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection));
					CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection), EDC_TileState::Hovered);
					if (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0)))->BuildingOnTile)
					{
						FullMoveDirection = CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0)))->BuildingOnTile->GetActorLocation();
						FullMoveDirection.Z = (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 275;
					}
					else
					{
						FullMoveDirection = OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0);
						FullMoveDirection.Z = (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 175;
						CustomPlayerController->Grid->GridVisual->addStateToTile(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection), EDC_TileState::Pathfinding);
					}
					UnitMovingCurrentMovNumber--;
				}
			} else
			{
				if (AllReachable.Contains(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection))
					&& Path.Num() < CustomPlayerController->UnitRef->GetPM()+1
					&& CustomPlayerController->Grid->GridPath->IsValidHeighWarrior(CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection)),CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection)))
					&& !CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection))->BuildingOnTile
					&& !Path.Contains(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection))
					)
				{
					Path.AddUnique(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection));
					CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection), EDC_TileState::Hovered);
					if (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0)))->BuildingOnTile)
					{
						FullMoveDirection = CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0)))->BuildingOnTile->GetActorLocation();
						FullMoveDirection.Z = (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 275;
					}
					else
					{
						FullMoveDirection = OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0);
						FullMoveDirection.Z = (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 175;
						CustomPlayerController->Grid->GridVisual->addStateToTile(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection), EDC_TileState::Pathfinding);
					}
					UnitMovingCurrentMovNumber--;
				}
			}
		}
		else if (IsSpawningUnit)
		{
			TArray<FIntPoint> AllReachable = CustomPlayerController->AllCurrentSpawnPoints;
			if (AllReachable.Contains(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection)))
			{
				CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection), EDC_TileState::Hovered);
				FullMoveDirection = OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0);
				FullMoveDirection.Z = (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 175;
				CustomPlayerController->Grid->GridVisual->addStateToTile(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection), EDC_TileState::Hovered);
			}
		}
		else if (IsAttacking || IsTowering)
		{
			TArray<FIntPoint> AllReachable = CustomPlayerController->GetPathReachable();
			if (AllReachable.Contains(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection)))
			{
				CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection), EDC_TileState::Hovered);
				if (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0)))->BuildingOnTile)
				{
					FullMoveDirection = CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0)))->BuildingOnTile->GetActorLocation();
					FullMoveDirection.Z = (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 275;
				}
				else
				{
					FullMoveDirection = OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0);
					FullMoveDirection.Z = (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 175;
					CustomPlayerController->Grid->GridVisual->addStateToTile(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection), EDC_TileState::Hovered);
				}
			}
		}
		else
		{
			CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection), EDC_TileState::Hovered);
			FullMoveDirection = OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0);
			FullMoveDirection.Z = (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 175;
			CustomPlayerController->Grid->GridVisual->addStateToTile(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection), EDC_TileState::Hovered);
		}

		CustomPlayerController->VerifyBuildInteraction();
	}
}

void ACameraPlayer::PathRemove(const FInputActionValue& Value)
{
	if (Path.Num() == 0)
	{
		IsAttacking = false;
		IsTowering = false;
		for(FIntPoint Point : CustomPlayerController->GetPathReachable())
		{
			CustomPlayerController->SetPlayerAction(EDC_ActionPlayer::None);
			CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(Point, EDC_TileState::Attacked);
		}
		if (CustomPlayerController)
		{
			if (CustomPlayerController->UnitRef)
			{
				FullMoveDirection.X = CustomPlayerController->UnitRef->GetActorLocation().X;
				FullMoveDirection.Y = CustomPlayerController->UnitRef->GetActorLocation().Y;
				FullMoveDirection.Z = CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->UnitRef->GetIndexPosition())->TileTransform.GetLocation().Z * 0.8 + 175;
				CustomPlayerController->VerifyBuildInteraction();
			}
		}
		return;
	}
	CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(Path.Last(), EDC_TileState::Pathfinding);
	Path.RemoveAt(Path.Num() - 1);
	if (!Path.IsEmpty())
	{
		if (Path.Last() != Path[0])
		{
			OldMoveDirection.X = Path.Last(1).X * 100;
			OldMoveDirection.Y = Path.Last(1).Y * 100;
			OldMoveDirection.Z = CustomPlayerController->Grid->GetGridData()->Find(Path.Last(1))->TileTransform.GetLocation().Z * 0.8 + 175;
		}
		UnitMovingCurrentMovNumber++;
		FullMoveDirection.X = Path.Last().X * 100;
		FullMoveDirection.Y = Path.Last().Y * 100;
		FullMoveDirection.Z = CustomPlayerController->Grid->GetGridData()->Find(Path.Last())->TileTransform.GetLocation().Z * 0.8 + 175;
	}

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
}

void ACameraPlayer::PathClear()
{
	Path.Empty();
}

	// ----------------------------
	// Camera Movement - TIMER

void ACameraPlayer::RepeatMoveTimerCamera(const FInputActionValue& Value)
{
	ValueInput = Value;
	MoveCamera();
	RepeatMoveTimerDelegate.BindUFunction(this, "MoveCamera", Value); 
	GetWorld()->GetTimerManager().SetTimer(
		RepeatMoveTimer,
		RepeatMoveTimerDelegate,
		0.15, 
		true,
		0.15); 
}

void ACameraPlayer::StopRepeatMoveTimerCamera()
{
	GetWorld()->GetTimerManager().ClearTimer(RepeatMoveTimer);
}

	// ----------------------------
	// Camera Rotation

void ACameraPlayer::RotateCamera(const FInputActionValue& Value)
{
	FVector2d Input = Value.Get<FVector2d>();

	TargetRotationYaw = FRotator(0,TargetRotationYaw.Yaw + UKismetMathLibrary::SignOfFloat(Input.X)*-90, 0);
	RotateWidget(UKismetMathLibrary::SignOfFloat(Input.X), GetActorLocation().Y);
}

void ACameraPlayer::RotateCameraPitch(const FInputActionValue& Value)
{
	FVector2d Input = Value.Get<FVector2d>();
	
	TargetRotationPitch = FRotator(UKismetMathLibrary::Clamp(TargetRotationPitch.Pitch + Input.Y, -75.0f, -20.0f),0, 0);
	RotateWidget(0, UKismetMathLibrary::SignOfFloat(Input.Y));
}

	// ----------------------------
	// Camera Zoom

void ACameraPlayer::ZoomCamera( const FInputActionValue& Value)
{
	const float Input = Value.Get<float>();

		if (Controller != nullptr)
		{
			CameraBoom->TargetArmLength = FMath::Clamp((CameraBoom->TargetArmLength + Input * ZoomCameraSpeed), 750.0f, 2050.0f);
		}
}

	// ----------------------------
	// Widget

void ACameraPlayer::RotateWidget_Implementation(float ValueX, float ValueY)
{
}

	// ----------------------------
	// Setter

void ACameraPlayer::SetCustomPlayerController(ACustomPlayerController* Cpc)
{
		CustomPlayerController = Cpc;
}