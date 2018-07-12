// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu.h"

#include "Components/Button.h"


bool UMainMenu::Initialize()
{
	// Super class returns a bool, so a check is needed.
	bool Success = Super::Initialize();
	if (!Success) return false;

	if (!ensure(Host != nullptr)) return false;
	Host->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

	return true;

}

void UMainMenu::SetMenuInterface(IMenuInterface* MenuInterface)
{
	// The val we are setting is named the same as the param passed in.
	//	Use this pointer to remove ambiguity.
	this->MenuInterface = MenuInterface;

}

void UMainMenu::HostServer()
{
	if (MenuInterface != nullptr)
	{
		MenuInterface->Host();

	}

}
