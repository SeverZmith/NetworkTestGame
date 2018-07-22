// Fill out your copyright notice in the Description page of Project Settings.

#include "MovingPlatform.h"


AMovingPlatform::AMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	SetMobility(EComponentMobility::Movable);

}

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	// If platform is the authority on server, replicate movement.
	if (HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);

	}

	// Set locations to patrol.
	GlobalStartLocation = GetActorLocation();
	GlobalTargetLocation = GetTransform().TransformPosition(TargetLocation);

}

void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ActiveTriggers > 0)
	{
		// HasAuthority() returns true if server, false if client
		if (HasAuthority())
		{
			// How far platform should travel, and how far it has travelled.
			FVector Location = GetActorLocation();
			float JourneyLength = (GlobalTargetLocation - GlobalStartLocation).Size();
			float JourneyTravelled = (Location - GlobalStartLocation).Size();

			// If travelled farther than designated length, flip around.
			if (JourneyTravelled >= JourneyLength)
			{
				FVector LocationToSwap = GlobalStartLocation;
				GlobalStartLocation = GlobalTargetLocation;
				GlobalTargetLocation = LocationToSwap;

			}

			// Set platform direction and speed.
			FVector Direction = (GlobalTargetLocation - GlobalStartLocation).GetSafeNormal();
			Location += PlatformSpeed * DeltaTime * Direction;
			SetActorLocation(Location);

		}
	}

}

void AMovingPlatform::AddActiveTrigger()
{
	ActiveTriggers++;

}

void AMovingPlatform::RemoveActiveTrigger()
{
	if (ActiveTriggers > 0)
	{
		ActiveTriggers--;

	}

}
