// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameMenu.h"

#include "Components/Button.h"
#include "GameFramework/GameModeBase.h"
#include "NetworkTestGameGameMode.h"


bool UInGameMenu::Initialize()
{
	// Super class returns a bool, so a check is needed.
	bool Success = Super::Initialize();
	if (!Success) return false;

	// Subscribe cancel button to UInGameMenu::CancelPressed.
	if (!ensure(CancelButton != nullptr)) return false;
	CancelButton->OnClicked.AddDynamic(this, &UInGameMenu::CancelPressed);

	// Subscribe quit button to UInGameMenu::QuitPressed.
	if (!ensure(QuitButton != nullptr)) return false;
	QuitButton->OnClicked.AddDynamic(this, &UInGameMenu::QuitPressed);

	return true;

}

void UInGameMenu::CancelPressed()
{
	// Remove in-game menu from screen.
	RemoveFromParent();

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	// Re-enable player controls.
	FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = false;

}

void UInGameMenu::QuitPressed()
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (!ensure(PlayerController != nullptr)) return;

	UWorld* World = PlayerController->GetWorld();
	if (!ensure(World != nullptr)) return;

	if (World->IsServer())
	{
		ANetworkTestGameGameMode* GameMode = World->GetAuthGameMode<ANetworkTestGameGameMode>();
		if (GameMode)
		{
			// If client that is quitting is the host, tell all other clients to return to lobby.
			GameMode->ReturnToMainMenuHost();

		}

	}
	else
	{
		if (PlayerController)
		{
			// Return client to main menu.
			PlayerController->ClientReturnToMainMenu("Back to Main Menu.");

		}

	}

}
