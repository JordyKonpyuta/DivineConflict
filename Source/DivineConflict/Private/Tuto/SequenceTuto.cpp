// Fill out your copyright notice in the Description page of Project Settings.


#include "Tuto/SequenceTuto.h"

#include "CustomGameState.h"
#include "CustomPlayerController.h"
#include "Grid.h"
#include "TutorialGameMode.h"
#include "Unit.h"
#include "Unit_Child_Mage.h"
#include "Unit_Child_Tank.h"
#include "Unit_Child_Warrior.h"

// Sets default values
ASequenceTuto::ASequenceTuto()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASequenceTuto::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(StartSequenceTimerHandle, this, &ASequenceTuto::StartSequence, 2.0f, false);
		
}

// Called every frame
void ASequenceTuto::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASequenceTuto::StartSequence()
{
	
	GameStateRef = Cast<ACustomGameState>(GetWorld()->GetGameState());
	TutorialGameModeRef = Cast<ATutorialGameMode>(GetWorld()->GetAuthGameMode());
	
		
	if (TutorialGameModeRef)
		TutorialGameModeRef->BeginTutorial();
	else GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Tutorial Game Mode Found"));

	GameStateRef->SequenceTutoRef = this;

	if (GridRef)
		NextSequence();
	else GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Grid Found"));
}

void ASequenceTuto::NextSequence()
{
	SequenceIndex++;

	switch (SequenceIndex)
	{
		case 1:
			Sequence1();
			break;
		case 2:
			TutorialGameModeRef->DisplayTutorialWidget(1);
			Sequence2();
			break;
		case 3:
			TutorialGameModeRef->DisplayTutorialWidget(2);
			Sequence3();
			break;
		case 4:
			TutorialGameModeRef->DisplayTutorialWidget(3);
			Sequence4();
			break;
		case 5:
			TutorialGameModeRef->DisplayTutorialWidget(4);
			Sequence5();
			break;
		case 6:
			TutorialGameModeRef->DisplayTutorialWidget(5);
			Sequence6();
			break;
		case 7:
			TutorialGameModeRef->DisplayTutorialWidget(6);
			Sequence7();
			break;
		case 8:
			TutorialGameModeRef->DisplayTutorialWidget(7);
			Sequence8();
			break;
		case 9:
			TutorialGameModeRef->DisplayTutorialWidget(8);
			Sequence9();
			break;
		default:	
			Sequence9();
			break;
	}
}

bool ASequenceTuto::IsSequenceFinished()
{
	switch (SequenceIndex)
	{
		case 1:
			return UnitKilled->GetCurrentHealth() < 1;
		case 2:
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("SpawnUnitSequence2: %d"), SpawnUnitSequence2));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("GetUnits: %d"), Cast<ACustomPlayerState>(GameStateRef->PlayerArray[0])->GetUnits()));
			return SpawnUnitSequence2 != Cast<ACustomPlayerState>(GameStateRef->PlayerArray[0])->GetUnits();
		case 3:
			return UnitKilled->GetCurrentHealth() < 1;
		case 4:
			return BuildingSequence4->GarrisonFull;
		case 5:
			return UnitKilled->GetCurrentHealth() < 1;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		default:
			break;
	}
	
	return false;
}

void ASequenceTuto::EndSequence()
{
	if (IsSequenceFinished())
	{
		if (UnitKilled)
			UnitKilled->Server_GetBuffs();
		if (UnitPlaying1)
			UnitPlaying1->Server_DestroyUnit();
		if (UnitPlaying2)
			UnitPlaying2->Server_DestroyUnit();
		
		NextSequence();
	}
	else
	{
		if (ACustomPlayerController* PlayerController = Cast<ACustomPlayerController>(GameStateRef->PlayerArray[0]->GetPlayerController()))
		{
			PlayerController->FailedTutorial();
			GetWorld()->GetAuthGameMode<ATutorialGameMode>()->isDead = true;
		}
	}
}

void ASequenceTuto::Sequence1()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Sequence 1"));
	UnitKilled = GetWorld()->SpawnActor<AUnit_Child_Tank>(GridRef->GetGridData()->Find(FIntPoint(8, 3))->TileTransform.GetLocation(), FRotator::ZeroRotator);
	UnitKilled->SetPlayerOwner(EPlayer::P_Heaven);
	UnitKilled->SetCurrentHealth(3);

	UnitPlaying1 =  GetWorld()->SpawnActor<AUnit_Child_Warrior>( GridRef->GetGridData()->Find(FIntPoint(10, 4))->TileTransform.GetLocation(), FRotator::ZeroRotator);
	UnitPlaying1->SetPlayerOwner(EPlayer::P_Hell);
	
}

void ASequenceTuto::Sequence2()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Sequence 2"));

	SpawnUnitSequence2 =  Cast<ACustomPlayerState>(GameStateRef->PlayerArray[0])->GetUnits();
}

void ASequenceTuto::Sequence3()
{
	GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Red, TEXT("Sequence 3"));

	UnitKilled = GetWorld()->SpawnActor<AUnit_Child_Tank>(GridRef->GetGridData()->Find(FIntPoint(14, 3))->TileTransform.GetLocation(), FRotator::ZeroRotator);
	UnitKilled->SetPlayerOwner(EPlayer::P_Heaven);
	UnitKilled->SetCurrentHealth(6);
	
	UnitPlaying1 = GetWorld()->SpawnActor<AUnit_Child_Mage>( GridRef->GetGridData()->Find(FIntPoint(15, 7))->TileTransform.GetLocation(), FRotator::ZeroRotator);
	UnitPlaying1->SetPlayerOwner(EPlayer::P_Hell);
	
	UnitPlaying2 = GetWorld()->SpawnActor<AUnit_Child_Mage>( GridRef->GetGridData()->Find(FIntPoint(9, 3))->TileTransform.GetLocation(), FRotator::ZeroRotator);
	UnitPlaying2->SetPlayerOwner(EPlayer::P_Hell);
}

void ASequenceTuto::Sequence4()
{
	GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Red, TEXT("Sequence 4"));

	if (ACustomPlayerState* PlayerState = Cast<ACustomPlayerState>(GameStateRef->PlayerArray[0]))
	{
		PlayerState->bIsActiveTurn = true;	
	}

	if (BuildingSequence4)
	{
		if (BuildingSequence4->GarrisonFull)
		{
			BuildingSequence4->UnitRef->Server_DestroyUnit();
			BuildingSequence4->UnitRef = nullptr;
			BuildingSequence4->GarrisonFull = false;
		}
	}

	UnitPlaying1 = GetWorld()->SpawnActor<AUnit_Child_Warrior>( GridRef->GetGridData()->Find(FIntPoint(9, 3))->TileTransform.GetLocation(), FRotator::ZeroRotator);
	UnitPlaying1->SetPlayerOwner(EPlayer::P_Hell);
	
}

void ASequenceTuto::Sequence5()
{	
	GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Red, TEXT("Sequence 5"));
}

void ASequenceTuto::Sequence6()
{	
	GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Red, TEXT("Sequence 6"));
}

void ASequenceTuto::Sequence7()
{	
	GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Red, TEXT("Sequence 7"));
}

void ASequenceTuto::Sequence8()
{	
	GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Red, TEXT("Sequence 8"));
}

void ASequenceTuto::Sequence9()
{	
	GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Red, TEXT("Sequence 9"));
}

