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

	if (HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}

}

void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// HasAuthority() returns true if server, false if client
	if (HasAuthority())
	{
		FVector Location = GetActorLocation();
		Location += FVector(PlatformSpeed * DeltaTime, 0.f, 0.f);
		SetActorLocation(Location);
	}

}
