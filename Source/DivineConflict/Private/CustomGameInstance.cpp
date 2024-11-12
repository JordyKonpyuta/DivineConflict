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
	SaveGameInstance = NewObject<UDC_SaveGameSystem>();
}

void UCustomGameInstance::SaveGame()
{
	SaveGameInstance->SaveSlotName = "Global";
	SaveGameInstance->PlayerLocation = Cast<ACameraPlayer>->GetActorLocation();
	SaveGameInstance->PlayerRotation = Cast<ACameraPlayer>->GetActorRotation();
	SaveGameInstance->CurrentUnits = Cast<ACustomPlayerState>->CurrentUnitCount;
	SaveGameInstance->MaxUnits = Cast<ACustomPlayerState>->MaxUnitCount;
	SaveGameInstance->bIsInActiveTurn = Cast<ACustomPlayerState>->bIsActiveTurn;
	SaveGameInstance->CurrentTurn = Cast<ACustomPlayerState>->TurnPassed;
	SaveGameInstance->PlayerGold = Cast<ACustomPlayerState>->GoldPoints;
	SaveGameInstance->PlayerWood = Cast<ACustomPlayerState>->WoodPoints;
	SaveGameInstance->PlayerStone = Cast<ACustomPlayerState>->StonePoints;

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
