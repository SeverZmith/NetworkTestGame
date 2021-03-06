// Fill out your copyright notice in the Description page of Project Settings.

#include "PuzzleGameInstance.h"

#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "UnrealNames.h"

#include "Triggers/PlatformTrigger.h"
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/MenuWidget.h"


const static FName SESSION_NAME = NAME_GameSession; // TODO verify remove: TEXT("GameSession");

const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");


UPuzzleGameInstance::UPuzzleGameInstance(const FObjectInitializer & ObjectInitializer)
{
	// Set MenuClass to WBP_MainMenu, and set InGameMenuClass to WBP_InGameManu.
	ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));

	if (!ensure(MenuBPClass.Class != nullptr)) return;
	MenuClass = MenuBPClass.Class;

	ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass(TEXT("/Game/MenuSystem/WBP_InGameMenu"));

	if (!ensure(InGameMenuBPClass.Class != nullptr)) return;
	InGameMenuClass = InGameMenuBPClass.Class;

}

void UPuzzleGameInstance::Init()
{
	Super::Init();

	// Get and subscribe to the online subsystem methods.
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found subsystem: %s"), *Subsystem->GetSubsystemName().ToString());
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPuzzleGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPuzzleGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPuzzleGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPuzzleGameInstance::OnJoinSessionComplete);

		}

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Found no subsystem."));

	}

}

void UPuzzleGameInstance::LoadMenuWidget()
{
	// Create the Menu object.
	if (!ensure(MenuClass != nullptr)) return;
	Menu = CreateWidget<UMainMenu>(this, MenuClass);
	if (!ensure(Menu != nullptr)) return;

	// This GameInstance implements the IMenuInterface, So we can pass 'this' pointer to SetMenuInterface().
	Menu->Setup();
	Menu->SetMenuInterface(this);

}

void UPuzzleGameInstance::InGameLoadMenu()
{
	// Create the Menu object.
	if (!ensure(MenuClass != nullptr)) return;
	UMenuWidget* InGameMenu = CreateWidget<UMenuWidget>(this, InGameMenuClass);
	if (!ensure(InGameMenu != nullptr)) return;

	// This GameInstance implements the IMenuInterface, So we can pass 'this' pointer to SetMenuInterface().
	InGameMenu->Setup();
	InGameMenu->SetMenuInterface(this);

}

void UPuzzleGameInstance::HostGame(FString ServerName)
{
	DesiredServerName = ServerName;
	if (SessionInterface.IsValid())
	{
		// If session exists, destroy it. Else, create it.
		FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if (ExistingSession != nullptr)
		{
			SessionInterface->DestroySession(SESSION_NAME);

		}
		else
		{
			CreateSession();

		}

	}

}

void UPuzzleGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success)
	{
		CreateSession();

	}

}

void UPuzzleGameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		// If subsystem is NULL (we are local), match with LAN connections.
		FOnlineSessionSettings SessionSettings;
		if (IOnlineSubsystem::Get()->GetSubsystemName() == NULL_SUBSYSTEM)
		{
			SessionSettings.bIsLANMatch = true;

		}
		else
		{
			SessionSettings.bIsLANMatch = false;

		}
		// Set session details.
		SessionSettings.NumPublicConnections = 5;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);

	}

}

void UPuzzleGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (!Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not create session!"));
		return;

	}

	UEngine* Engine = GetEngine();

	if (!ensure(Engine != nullptr)) return;
	Engine->AddOnScreenDebugMessage(-1, 5, FColor::Green, TEXT("Hosting"));

	UWorld* World = GetWorld();

	if (!ensure(World != nullptr)) return;
	World->ServerTravel("/Game/PuzzleGame/Maps/Lobby?listen");

}

void UPuzzleGameInstance::RefreshServerList()
{
	// Search for session over AppID.
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		/** Restrict search results to LAN only. */
		// SessionSearch->bIsLanQuery = true;

		SessionSearch->MaxSearchResults = 100; // Setting this allows the search to find our game. this is because we share an AppID until we pay Steam deposit.
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

		UE_LOG(LogTemp, Warning, TEXT("Starting Find Session..."));
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());

	}

}

void UPuzzleGameInstance::OnFindSessionsComplete(bool Success)
{	
	if (Success && SessionSearch.IsValid() && Menu != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Finishing Find Session..."));

		TArray<FServerData> ServerNames;
		// For each session found.
		for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found session names: %s"), *SearchResult.GetSessionIdStr());
			// Set server details.
			FServerData Data;
			Data.Name = SearchResult.GetSessionIdStr();
			Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
			Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
			Data.HostUsername = SearchResult.Session.OwningUserName;
			FString ServerName;
			if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))
			{
				Data.Name = ServerName;

			}
			else
			{
				Data.Name = "Could not find name.";

			}
			ServerNames.Add(Data);

		}
		Menu->SetServerList(ServerNames);

	}

}

void UPuzzleGameInstance::JoinGame(uint32 Index)
{
	if (!SessionInterface.IsValid()) return;
	if (!SessionSearch.IsValid()) return;

	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);

}

void UPuzzleGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid()) return;

	FString Address;
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not get connect string."));
		return;

	}

	UEngine* Engine = GetEngine();

	if (!ensure(Engine != nullptr)) return;
	Engine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, FString::Printf(TEXT("Joining: %s"), *Address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	
	if (!ensure(PlayerController != nullptr)) return;
	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);

}

void UPuzzleGameInstance::StartSession()
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->StartSession(SESSION_NAME);

	}

}
