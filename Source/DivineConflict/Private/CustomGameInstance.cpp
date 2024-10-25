// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameInstance.h"
#include "InputCoreTypes.h"

UCustomGameInstance::UCustomGameInstance()
{
	EnableFaceBottomKey(true);
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
