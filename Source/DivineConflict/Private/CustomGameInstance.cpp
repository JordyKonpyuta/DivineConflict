// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameInstance.h"
#include "InputCoreTypes.h"

UCustomGameInstance::UCustomGameInstance()
{
	FKey& virtual_key = const_cast<FKey&>(EKeys::Virtual_Accept);
	virtual_key = EKeys::Invalid;
}