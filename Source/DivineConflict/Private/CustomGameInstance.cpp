// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameInstance.h"

#include "CameraPlayer.h"
#include "CustomPlayerState.h"
#include "DC_SaveGameSystem.h"
#include "InputCoreTypes.h"
#include "Unit.h"
#include "Kismet/GameplayStatics.h"

UCustomGameInstance::UCustomGameInstance()
{
	EnableFaceBottomKey(true);
}

void UCustomGameInstance::Init()
{
	Super::Init();
}

void UCustomGameInstance::SaveGame()
{
	SaveGameInstance->SaveSlotName = "Global";
	SaveGameInstance->PlayerLocation = CameraPlayer->GetActorLocation();
	SaveGameInstance->PlayerRotation = CameraPlayer->GetActorRotation();
	SaveGameInstance->CurrentUnits = CustomPlayerState->GetUnits();
	SaveGameInstance->MaxUnits = CustomPlayerState->GetMaxUnits();
	SaveGameInstance->bIsInActiveTurn = CustomPlayerState->bIsActiveTurn;
	SaveGameInstance->CurrentTurn = CustomPlayerState->TurnPassed;
	SaveGameInstance->PlayerGold = CustomPlayerState->GetGoldPoints();
	SaveGameInstance->PlayerWood = CustomPlayerState->GetWoodPoints();
	SaveGameInstance->PlayerStone = CustomPlayerState->GetStonePoints();

	TArray<AActor*> Units;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnit::StaticClass(), Units);

	for (AActor* Unit : Units)
	{
		if (AUnit* CastedUnit = Cast<AUnit>(Unit))
		{
			if (CastedUnit->GetPlayerOwner() == EPlayer::P_Hell)
			{
				SaveGameInstance->HellUnits.Add(CastedUnit);
			}
			else if (CastedUnit->GetPlayerOwner() == EPlayer::P_Heaven)
			{
				SaveGameInstance->HeavenUnits.Add(CastedUnit);
			}

			else
			{
				SaveGameInstance->NeutralUnits.Add(CastedUnit);
			}
		}
	}
}

void UCustomGameInstance::EnableFaceBottomKey(bool enable)
{
	FKey& virtual_key = const_cast<FKey&>(EKeys::Virtual_Accept);
	if (enable)
	{
		virtual_key = EKeys::Gamepad_FaceButton_Bottom;
	}
	else
	{
		virtual_key = EKeys::Invalid;
	}
}
