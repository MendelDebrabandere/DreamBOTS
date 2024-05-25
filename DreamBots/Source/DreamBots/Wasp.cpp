// Fill out your copyright notice in the Description page of Project Settings.


#include "Wasp.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkAudioEvent.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AWasp::AWasp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WaspRoot = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	WaspRoot->SetupAttachment(RootComponent);


	CollisionBox = CreateDefaultSubobject<UBoxComponent>(FName("CollisionBox"));
	CollisionBox->SetupAttachment(WaspRoot);

	NiagaraComponent = CreateDefaultSubobject< UNiagaraComponent>(FName("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(WaspRoot);
	NiagaraSystem = NiagaraComponent->GetAsset();

	// Create AkComponent for audio
	WaspAudio = CreateDefaultSubobject<UAkComponent>(TEXT("WaspAudio"));
	WaspAudio->SetupAttachment(WaspRoot);

}

void AWasp::GetHit()
{
	// Wwise
	// Wasp hit sound
	FOnAkPostEventCallback nullCallback{};
	WEHit->PostOnActor(this, nullCallback, 0, false);

	if (CurrentHealth <= 1.f)
	{
		Destroy();
	}
	else
	{

		HealthNiagaraSpawnRate /= (float)CurrentHealth;
		NiagaraComponent->SetFloatParameter("SpawnRate", HealthNiagaraSpawnRate);
		float niagaraSpawn = HealthNiagaraSpawnRate;
		--CurrentHealth;
	}
}

// Called when the game starts or when spawned
void AWasp::BeginPlay()
{
	Super::BeginPlay();
	++WaspCounter;

	CurrentHealth = MaxHealth;
	if (NiagaraSystem)
	{
		NiagaraComponent->Activate();
	}
	NiagaraScale = NiagaraComponent->GetRelativeScale3D();
	NiagaraComponent->SetFloatParameter("SpawnRate", NiagaraStartSpawnRate);
	HealthNiagaraSpawnRate = NiagaraStartSpawnRate;
	WaspAudio->PostAkEvent(WEIdle);
}

void AWasp::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	--WaspCounter;
	WaspAudio->PostAkEvent(WEDeath);
}



// Called every frame
void AWasp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

