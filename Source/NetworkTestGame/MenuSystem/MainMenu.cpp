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

void UMainMenu::Setup()
{
	// Add Menu to screen.
	this->AddToViewport();

	// Check World nullptr.
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	// Get player controller from world.
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	// Get the slate widget with TakeWidget() and set is as widget to focus on.
	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(this->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	// Set our input mode with the constructed data.
	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = true;

}

void UMainMenu::HostServer()
{
	if (MenuInterface != nullptr)
	{
		MenuInterface->Host();

	}

}

// Gets called when widget gets removed from the world (level changes)
void UMainMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);

	this->RemoveFromViewport();

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = false;

}
