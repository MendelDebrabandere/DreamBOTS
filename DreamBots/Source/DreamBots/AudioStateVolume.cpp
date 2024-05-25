// Fill out your copyright notice in the Description page of Project Settings.


#include "AudioStateVolume.h"
#include "DreamBotsCharacter.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkStateValue.h"

// Sets default values
AAudioStateVolume::AAudioStateVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	Collider->SetupAttachment(RootComponent);
	Collider->OnComponentBeginOverlap.AddDynamic(this, &AAudioStateVolume::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AAudioStateVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAudioStateVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!State)
		return;

	ADreamBotsCharacter* PlayerCharacter = Cast<ADreamBotsCharacter>(OtherActor);

	if (PlayerCharacter)
	{
		UAkGameplayStatics::SetState(State, StateGroupName, StateName);
	}
}

// Called every frame
void AAudioStateVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

