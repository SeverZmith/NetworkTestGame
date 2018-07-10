// Fill out your copyright notice in the Description page of Project Settings.

#include "PlatformTrigger.h"

#include "Components/BoxComponent.h"
#include "Obstacles/MovingPlatform.h"


// Sets default values
APlatformTrigger::APlatformTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(FName("TriggerVolume"));

	if (!ensure(TriggerVolume != nullptr)) return;
	RootComponent = TriggerVolume;

}

// Called when the game starts or when spawned
void APlatformTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(TriggerVolume != nullptr)) return;
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APlatformTrigger::OnOverlapBegin);

	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &APlatformTrigger::OnOverlapEnd);
	
}

// Called every frame
void APlatformTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlatformTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	/** Shows log of Server or Client depending on player (host is server) */
	/// FString Type = HasAuthority() ? FString("Server") : FString("Client");
	/// UE_LOG(LogTemp, Warning, TEXT("[%s]: PlatformTrigger Activated."), *Type);

	for (AMovingPlatform* Platform : PlatformsToTrigger)
	{
		Platform->AddActiveTrigger();
	}

}

void APlatformTrigger::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/** Shows log of Server or Client depending on player (host is server) */
	/// FString Type = HasAuthority() ? FString("Server") : FString("Client");
	/// UE_LOG(LogTemp, Warning, TEXT("[%s]: PlatformTrigger Deactivated."), *Type);

	for (AMovingPlatform* Platform : PlatformsToTrigger)
	{
		Platform->RemoveActiveTrigger();
	}

}
