// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPlayer.h"
#include "CustomPlayerController.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Grid.h"
#include "GridPath.h"
#include "GridVisual.h"
#include "ScreenPass.h"
#include "Unit.h"
#include "Unit_Child_Warrior.h"
#include "DSP/WaveTableOsc.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/SpringArmComponent.h"

	// ----------------------------
	// Constructor

ACameraPlayer::ACameraPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
	
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
					&& (!CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection))->BuildingOnTile || FirstMove)
					&& (!Path.Contains(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection)) || FirstMove)
					)
				{
					if (!CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection))->BuildingOnTile)
					{
						if (FirstMove)
							FirstMove = false;

						Path.AddUnique(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection));
						CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection), EDC_TileState::Hovered);
					}

					if (!CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection))->BuildingOnTile)
						FirstMove = false;

					if (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0)))->BuildingOnTile && !FirstMove)
					{
						FullMoveDirection = CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0)))->BuildingOnTile->GetActorLocation();
						FullMoveDirection.Z = (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 275;
						CustomPlayerController->Grid->GridVisual->addStateToTile(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection), EDC_TileState::Pathfinding);
						Path.AddUnique(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection));
					}
					else
					{
						FullMoveDirection = OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0);
						FullMoveDirection.Z = (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 175;
						CustomPlayerController->Grid->GridVisual->addStateToTile(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection), EDC_TileState::Pathfinding);
					}
					UnitMovingCurrentMovNumber--;
				}
				else if (AllReachable.Contains(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection))
						 && (Path.Contains(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection)) || ( FirstMove && CustomPlayerController->UnitRef->GetIsGarrison())))
				{
					FullMoveDirection = OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0);
					FullMoveDirection.Z = (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 175;
					
					for (int CurrentIndex = Path.Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection)) + 1;
							 CurrentIndex < Path.Num();
							 CurrentIndex++)
					{
						CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(Path[CurrentIndex], EDC_TileState::Pathfinding);
					}
					Path.RemoveAt(Path.Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection)), Path.Num() - Path.Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection)), true);
					Path.AddUnique(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection));
					CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection), EDC_TileState::Hovered);
				}
			} else
			{
				if (AllReachable.Contains(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection))
					&& Path.Num() < CustomPlayerController->UnitRef->GetPM()+1
					&& CustomPlayerController->Grid->GridPath->IsValidHeighWarrior(CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection)),CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection)))
					&& (!CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection))->BuildingOnTile || FirstMove)
					&& (!Path.Contains(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection)) || FirstMove)
					)
				{
					if (!CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection))->BuildingOnTile)
					{
						if (FirstMove)
							FirstMove = false;

						Path.AddUnique(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection));
						CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection), EDC_TileState::Hovered);
					}

					if (!CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection))->BuildingOnTile)
						FirstMove = false;
					
					if (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0)))->BuildingOnTile && !FirstMove)
					{
						FullMoveDirection = CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0)))->BuildingOnTile->GetActorLocation();
						FullMoveDirection.Z = (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 275;
						Path.AddUnique(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection));
						CustomPlayerController->Grid->GridVisual->addStateToTile(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection), EDC_TileState::Pathfinding);
					}
					else
					{
						FullMoveDirection = OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0);
						FullMoveDirection.Z = (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 175;
						CustomPlayerController->Grid->GridVisual->addStateToTile(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection), EDC_TileState::Pathfinding);
					}
					UnitMovingCurrentMovNumber--;
				}
				else if (AllReachable.Contains(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection))
						 && (Path.Contains(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection))
						 	|| ( FirstMove && CustomPlayerController->UnitRef->GetIsGarrison())))
				{
					
					FullMoveDirection = OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0);
					FullMoveDirection.Z = (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 175;

					for (int CurrentIndex = Path.Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection)) + 1;
							 CurrentIndex < Path.Num();
							 CurrentIndex++)
					{
						CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(Path[CurrentIndex], EDC_TileState::Pathfinding);
					}
					
					Path.RemoveAt(Path.Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection)), Path.Num() - Path.Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection)), true);
					Path.AddUnique(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection));
					CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection), EDC_TileState::Hovered);
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
		else if (IsAttacking)
		{
			TArray<FIntPoint> AllReachable = CustomPlayerController->GetPathReachable();
			if (!CustomPlayerController->UnitRef->HasMoved)
			{
				if (AllReachable.Contains(CustomPlayerController->Grid->ConvertLocationToIndex(CustomPlayerController->UnitRef->GetActorLocation() + MoveDirection)))
				{
					CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection), EDC_TileState::Hovered);
					if (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0)))->BuildingOnTile)
					{
						FullMoveDirection = CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0)))->BuildingOnTile->GetActorLocation();
						FullMoveDirection.Z = (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 275;
					}
					else
					{
						FullMoveDirection = CustomPlayerController->UnitRef->GetActorLocation() + FVector(MoveDirection.X,MoveDirection.Y, 0);
						FullMoveDirection.Z = (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 175;
						CustomPlayerController->Grid->GridVisual->addStateToTile(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection), EDC_TileState::Hovered);
					}
				}
			}
			else
			{
				if (AllReachable.Contains(CustomPlayerController->Grid->ConvertLocationToIndex(CustomPlayerController->UnitRef->GetFinalGhostMesh()->GetComponentLocation() + MoveDirection)))
				{
					CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection), EDC_TileState::Hovered);
					if (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0)))->BuildingOnTile)
					{
						FullMoveDirection = CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0)))->BuildingOnTile->GetActorLocation();
						FullMoveDirection.Z = (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 275;
					}
					else
					{
						FullMoveDirection = CustomPlayerController->UnitRef->GetFinalGhostMesh()->GetComponentLocation() + FVector(MoveDirection.X,MoveDirection.Y, 0);
						FullMoveDirection.Z = (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 175;
						CustomPlayerController->Grid->GridVisual->addStateToTile(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection), EDC_TileState::Hovered);
					}
				}	
			}
		}
		else if (IsTowering || IsSpelling)
		{
			TArray<FIntPoint> AllReachable = CustomPlayerController->GetPathReachable();
			if (AllReachable.Contains(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection + MoveDirection)))
			{
				CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection), EDC_TileState::Hovered);
				FullMoveDirection = OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0);
				FullMoveDirection.Z = (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 175;
				CustomPlayerController->Grid->GridVisual->addStateToTile(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection), EDC_TileState::Hovered);
			}
		}
		else
		{
			CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(CustomPlayerController->Grid->ConvertLocationToIndex(OldMoveDirection), EDC_TileState::Hovered);
			FullMoveDirection = OldMoveDirection + FVector(MoveDirection.X,MoveDirection.Y, 0);
			FullMoveDirection.Z = (CustomPlayerController->Grid->GetGridData()->Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->TileTransform.GetLocation().Z * 0.8) + 175;
			CustomPlayerController->Grid->GridVisual->addStateToTile(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection), EDC_TileState::Hovered);
			
			if (CustomPlayerController->Grid->GridData.Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->UnitOnTile)
			{
				CustomPlayerController->UnitRef = CustomPlayerController->Grid->GridData.Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->UnitOnTile;
				CustomPlayerController->bHoveringOverUnit = true;
				CustomPlayerController->DisplayWidgetHovering();
			}
			else if (CustomPlayerController->Grid->GridData.Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->BaseOnTile)
			{
				CustomPlayerController->BaseRef = CustomPlayerController->Grid->GridData.Find(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection))->BaseOnTile;
				CustomPlayerController->bHoveringOverBase = true;
				CustomPlayerController->DisplayWidgetHovering();
			}
			else
			{
				if (CustomPlayerController->bHoveringOverUnit || CustomPlayerController->bHoveringOverBase)
				{
					CustomPlayerController->bHoveringOverBase = false;	
					CustomPlayerController->bHoveringOverUnit = false;
					CustomPlayerController->DisplayWidgetHovering();
				}
			}
		}
		CustomPlayerController->VerifyBuildInteraction();
		CustomPlayerController->InactivityTimer = 0;
		CustomPlayerController->DisplayInputsOnHUD();
	}
}

void ACameraPlayer::CancelAction(const FInputActionValue& Value)
{
	if (CustomPlayerController->GetPlayerAction() != EDC_ActionPlayer::None)
	{
		// Cancel Action Type
		
		IsAttacking = false;
		IsTowering = false;
		IsSpelling = false;

		// Clear Path and Pathfinding
		
		for(FIntPoint Point : CustomPlayerController->GetPathReachable())
		{
			CustomPlayerController->SetPlayerAction(EDC_ActionPlayer::None);
			CustomPlayerController->Grid->GridVisual->RemoveAllStateFromTile(Point);
		}

		// Prepare Camera

		CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection), EDC_TileState::Hovered);
		
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
		
		CustomPlayerController->Grid->GridVisual->addStateToTile(CustomPlayerController->Grid->ConvertLocationToIndex(FullMoveDirection), EDC_TileState::Hovered);
		
		OldMoveDirection = FullMoveDirection;

		// Remove Action Type

		Path.Empty();
		CustomPlayerController->SetPlayerAction(EDC_ActionPlayer::None);
		IsMovingUnit = false;
	}
	else
	{		
		CustomPlayerController->Server_CancelLastAction();
	}
}

void ACameraPlayer::PathClear()
{
	Path.Empty();
}

void ACameraPlayer::ClearMoveMode()
{
	IsMovingUnit = false;
	IsAttacking = false;
	IsTowering = false;
	IsSpawningUnit = false;
	IsSpelling = false;
	for(FIntPoint Point : Path)
	{
		CustomPlayerController->Grid->GridVisual->RemoveStateFromTile(Point, EDC_TileState::Pathfinding);
	}
	PathClear();
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

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("UpdatedMove"));

	TargetRotationYaw = FRotator(0,TargetRotationYaw.Yaw + UKismetMathLibrary::SignOfFloat(Input.X)*-90, 0);
	RotateWidget();
}

void ACameraPlayer::RotateCameraPitch(const FInputActionValue& Value)
{
	FVector2d Input = Value.Get<FVector2d>();
	
	TargetRotationPitch = FRotator(UKismetMathLibrary::Clamp(TargetRotationPitch.Pitch + Input.Y, -75.0f, -20.0f),0, 0);
	RotateWidget();
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

void ACameraPlayer::RotateWidget_Implementation()
{
}

	// ----------------------------
	// Setter

void ACameraPlayer::SetCustomPlayerController(ACustomPlayerController* Cpc)
{
		CustomPlayerController = Cpc;
}