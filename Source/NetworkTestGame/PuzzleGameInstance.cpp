// Fill out your copyright notice in the Description page of Project Settings.

#include "PuzzleGameInstance.h"

#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

#include "Triggers/PlatformTrigger.h"
#include "MenuSystem/MainMenu.h"


UPuzzleGameInstance::UPuzzleGameInstance(const FObjectInitializer & ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));

	if (!ensure(MenuBPClass.Class != nullptr)) return;
	MenuClass = MenuBPClass.Class;

}

void UPuzzleGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogTemp, Warning, TEXT("Found class: %s"), *MenuClass->GetName());

}

void UPuzzleGameInstance::LoadMenu()
{
	// Create the Menu object.
	if (!ensure(MenuClass != nullptr)) return;
	Menu = CreateWidget<UMainMenu>(this, MenuClass);
	if (!ensure(Menu != nullptr)) return;

	// This GameInstance implements the IMenuInterface, So we can pass 'this' pointer to SetMenuInterface().
	Menu->Setup();
	Menu->SetMenuInterface(this);

}

void UPuzzleGameInstance::Host()
{
	UEngine* Engine = GetEngine();

	if (!ensure(Engine != nullptr)) return;
	Engine->AddOnScreenDebugMessage(-1, 5, FColor::Green, TEXT("Hosting"));

	UWorld* World = GetWorld();

	if (!ensure(World != nullptr)) return;
	World->ServerTravel("/Game/ThirdPersonCPP/Maps/ThirdPersonExampleMap?listen");

}

void UPuzzleGameInstance::Join(const FString& Address)
{
	UEngine* Engine = GetEngine();

	if (!ensure(Engine != nullptr)) return;
	Engine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, FString::Printf(TEXT("Joining: %s"), *Address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	
	if (!ensure(PlayerController != nullptr)) return;
	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);

}
