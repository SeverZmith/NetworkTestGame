// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetworkTestGameGameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKTESTGAME_API ALobbyGameMode : public ANetworkTestGameGameMode
{
	GENERATED_BODY()
	
public:
	/** Called after a successful login.  This is the first place it is safe to call replicated functions on the PlayerController. */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/** Called when a Controller with a PlayerState leaves the game or is destroyed */
	virtual void Logout(AController* Exiting) override;

private:
	void StartGame();

	uint32 NumberOfPlayers = 0;

	FTimerHandle GameStartTimer;

};
