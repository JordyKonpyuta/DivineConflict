// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "CustomGameState.h"
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
	
	if(CameraPlayerRef != nullptr)
	{
		CameraPlayerRef->SetCustomPlayerController(this);
	}
	
	// Send Controller to Instance
	if(ACustomGameState* GameState = Cast<ACustomGameState>(GetWorld()->GetGameState()))
	{
		GameState->PlayerControllers.Add(this);
	}

	setGrid();
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

void ACustomPlayerController::setGrid()
{
	//get the grid
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Set Grid"));
	AActor* FoundActors = UGameplayStatics::GetActorOfClass(GetWorld(), AGrid::StaticClass());
	if (FoundActors != nullptr)
	{
		Grid = Cast<AGrid>(FoundActors);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Grid Found"));
		//delay 0.2s and try again
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACustomPlayerController::setGrid, 0.2f, false);
		
	}
}

void ACustomPlayerController::ControllerInteration()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Interact Controller"));
	if(Grid != nullptr)
	{
		if(UnitRef != nullptr)
        {
            CameraPlayerRef->IsMovingUnit = false;
			UnitRef->SetIsSelected(false);
			UnitRef->Move();
			UnitRef = nullptr;
        }
		
		FIntPoint PlayerPositionInGrid = Grid->ConvertLocationToIndex(CameraPlayerRef->GetActorLocation());
		if(Grid->GetGridData()->Find(PlayerPositionInGrid) != nullptr)
		{
			if(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile != nullptr)
			{

				UnitRef = Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile;
				CameraPlayerRef->SetCustomPlayerController(this);
				IInteractInterface::Execute_Interact(Grid->GetGridData()->Find(PlayerPositionInGrid)->UnitOnTile, this);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Unit Found"));
				
				
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

void ACustomPlayerController::SetIsHell(bool bH)
{
	IsHell = bH;
}

bool ACustomPlayerController::GetIsInActiveTurn()
{
	return IsInActiveTurn;
}

void ACustomPlayerController::SetIsInActiveTurn(bool bA)
{
	IsInActiveTurn = bA;
}

bool ACustomPlayerController::GetIsReady()
{
	return IsReady;
}

void ACustomPlayerController::SetIsReady(bool bR)
{
	IsReady = bR;
}

bool ACustomPlayerController::GetIsDead()
{
	return IsDead;
}

void ACustomPlayerController::SetIsDead(bool bD)
{
	IsDead = bD;
}
