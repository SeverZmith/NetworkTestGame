// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

#include "ServerRow.h"


UMainMenu::UMainMenu(const FObjectInitializer & ObjectInitializer)
{
	// Connecting widget blueprint to our ServerRowClass.
	ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/MenuSystem/WBP_ServerRow"));

	if (!ensure(ServerRowBPClass.Class != nullptr)) return;
	ServerRowClass = ServerRowBPClass.Class;

}

bool UMainMenu::Initialize()
{
	// Super class returns a bool, so a check is needed.
	bool Success = Super::Initialize();
	if (!Success) return false;

	// Subscribe buttons to appropriate UMainMenu methods.
	if (!ensure(HostButton != nullptr)) return false;
	HostButton->OnClicked.AddDynamic(this, &UMainMenu::OpenHostMenu);

	if (!ensure(JoinButton != nullptr)) return false;
	JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	if (!ensure(QuitButton != nullptr)) return false;
	QuitButton->OnClicked.AddDynamic(this, &UMainMenu::QuitPressed);

	if (!ensure(ConfirmHostButton != nullptr)) return false;
	ConfirmHostButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

	if (!ensure(CancelHostButton != nullptr)) return false;
	CancelHostButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	if (!ensure(ConfirmJoinMenuButton != nullptr)) return false;
	ConfirmJoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);

	if (!ensure(CancelJoinMenuButton != nullptr)) return false;
	CancelJoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	return true;

}

void UMainMenu::OpenMainMenu()
{
	if (!ensure(MenuWidgetSwitcher != nullptr)) return;
	if (!ensure(MainMenu != nullptr)) return;

	// Menus are attached to the WidgetSwitcher. Here we are setting the active widget to the main menu.
	MenuWidgetSwitcher->SetActiveWidget(MainMenu);

}

void UMainMenu::OpenHostMenu()
{
	MenuWidgetSwitcher->SetActiveWidget(HostMenu);

}

void UMainMenu::OpenJoinMenu()
{
	if (!ensure(MenuWidgetSwitcher != nullptr)) return;
	if (!ensure(JoinMenu != nullptr)) return;

	MenuWidgetSwitcher->SetActiveWidget(JoinMenu);

	// When opening the join menu, refresh the server list.
	if (MenuInterface != nullptr)
	{
		MenuInterface->RefreshServerList();

	}

}

void UMainMenu::HostServer()
{
	if (MenuInterface != nullptr)
	{
		// Get the server name from the editable text box, and host a game with that name.
		FString ServerName = HostServerName->Text.ToString();
		MenuInterface->HostGame(ServerName);

	}

}

void UMainMenu::JoinServer()
{
	// If the server row has been selected...
	if (SelectedIndex.IsSet() && MenuInterface != nullptr)
	{
		// ...join game at the selected index.
		UE_LOG(LogTemp, Warning, TEXT("Selected index %d"), SelectedIndex.GetValue());
		MenuInterface->JoinGame(SelectedIndex.GetValue());

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index not set."));

	}

}

void UMainMenu::QuitPressed()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	// Quit the game.
	PlayerController->ConsoleCommand("quit");

}

void UMainMenu::SetServerList(TArray<FServerData> ServerNames)
{
	UWorld* World = this->GetWorld();
	if (!ensure(World != nullptr)) return;

	// Clear the list so it can be refreshed.
	ServerList->ClearChildren();

	uint32 i = 0;
	for (const FServerData& ServerData : ServerNames)
	{
		UServerRow* Row = CreateWidget<UServerRow>(World, ServerRowClass);
		if (!ensure(Row != nullptr)) return;

		// Set row information.
		Row->ServerName->SetText(FText::FromString(ServerData.Name));
		Row->HostUser->SetText(FText::FromString(ServerData.HostUsername));
		FString FractionText = FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers);
		Row->ConnectionFraction->SetText(FText::FromString(FractionText));
		Row->Setup(this, i);
		++i;

		// Add row to list.
		ServerList->AddChild(Row);

	}

}

void UMainMenu::SelectIndex(uint32 Index)
{
	// Update selected game index.
	SelectedIndex = Index;
	UpdateChildren();

}

void UMainMenu::UpdateChildren()
{
	// Get rows of the server list.
	for (int32 i = 0; i < ServerList->GetChildrenCount(); ++i)
	{
		UServerRow* Row = Cast<UServerRow>(ServerList->GetChildAt(i));
		if (Row != nullptr)
		{
			// If val on left of && is false it doesn't bother with whatever is to the right of it.
			Row->Selected = (SelectedIndex.IsSet() && SelectedIndex.GetValue() == i);

		}

	}

}
