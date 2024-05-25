// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSwapZone.h"

#include "DreamBotsCharacter.h"
#include "Components/BoxComponent.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkAudioEvent.h"

// Sets default values
ACharacterSwapZone::ACharacterSwapZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	Root->SetupAttachment(RootComponent);

	BoxComp = CreateDefaultSubobject<UBoxComponent>(FName("Box"));
	BoxComp->SetupAttachment(Root);

}

// Called when the game starts or when spawned
void ACharacterSwapZone::BeginPlay()
{
	Super::BeginPlay();

	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ACharacterSwapZone::OnBeginOverlap);

}

void ACharacterSwapZone::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the Overlapping Actor is the Player Character
	ADreamBotsCharacter* PlayerCharacter = Cast<ADreamBotsCharacter>(OtherActor);


	if (PlayerCharacter)
	{
		PlayerCharacter->GoBackToOldType();
	}

	if (WESwapAudioEvent)
	{
		// Wwise
		// Wasp hit sound
		FOnAkPostEventCallback nullCallback{};
		WESwapAudioEvent->PostOnActor(this, nullCallback, 0, false);
	}
}

// Called every frame
void ACharacterSwapZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

