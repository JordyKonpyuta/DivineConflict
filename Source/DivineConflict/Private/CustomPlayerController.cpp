// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Engine\LocalPlayer.h"
#include "CameraPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Grid.h"
#include "Unit.h"
#include "Kismet/KismetMathLibrary.h"


void ACustomPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CameraPlayerRef = Cast<ACameraPlayer>(GetPawn());

	CameraPlayerRef->setCustomePlayerController(this);

	//get the grid
	AActor* FoundActors;
	FoundActors = UGameplayStatics::GetActorOfClass(GetWorld(), AGrid::StaticClass());
	if (FoundActors != nullptr)
	{
		Grid = Cast<AGrid>(FoundActors);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Grid Found"));
	}

	

	
}

void ACustomPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (APlayerController* PlayerController = Cast<APlayerController>(this))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
}
void ACustomPlayerController::ControllerInteration()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Interact Controller"));
	if(Grid != nullptr)
	{
		
		FIntPoint PlayerPositionInGrid = Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation());
		if(Grid->GetGridData().Find(PlayerPositionInGrid) != nullptr)
		{
			
			if(Grid->GetGridData().Find(PlayerPositionInGrid)->UnitOnTile != nullptr)
			{
				if(CameraPlayerRef->IsMovingUnit)
				{
					CameraPlayerRef->IsMovingUnit = false;
					Grid->GetGridData().Find(PlayerPositionInGrid)->UnitOnTile->Move();
					
				}
				else
				{
					IInteractInterface::Execute_Interact(Grid->GetGridData().Find(PlayerPositionInGrid)->UnitOnTile, this);
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Unit Found"));
				}
				
			}

		}
	}	
	
	
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
