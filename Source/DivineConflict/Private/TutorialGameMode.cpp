// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialGameMode.h"

#include "Grid.h"
#include "GridVisual.h"
#include "Unit.h"
#include "Kismet/GameplayStatics.h"

void ATutorialGameMode::BeginPlay()
{
	Super::BeginPlay();

	AActor* FoundActor;
	FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), AGrid::StaticClass());
	Grid = Cast<AGrid>(FoundActor);
	BeginTutorial();
}

void ATutorialGameMode::HilightUnit(AUnit* UnitToHilight)
{
	if (UnitToHilight == nullptr)
    {
        return;
    }
	Grid->GridVisual->addStateToTile(Grid->ConvertLocationToIndex(UnitToHilight->GetActorLocation()), EDC_TileState::Selected);
}

void ATutorialGameMode::DisplayTutorialWidget_Implementation(int index)
{
}

void ATutorialGameMode::BeginTutorial_Implementation()
{
}
