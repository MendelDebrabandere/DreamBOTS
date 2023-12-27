// Fill out your copyright notice in the Description page of Project Settings.


#include "DiscoFloor.h"
#include "DreamBotsCharacter.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkStateValue.h"

// Sets default values
ADiscoFloor::ADiscoFloor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	StartBox = CreateDefaultSubobject<UBoxComponent>(TEXT("StartBox"));
	StartBox->SetupAttachment(Root);
	StartBox->OnComponentBeginOverlap.AddDynamic(this, &ADiscoFloor::OnBeginOverlap);

	
}

// Called when the game starts or when spawned
void ADiscoFloor::BeginPlay()
{
	Super::BeginPlay();
	
	SecPerBeat = 60.0 / BPM;
	SecPerTextureChange = SecPerBeat * TriggerPerBeat;

	UStaticMeshComponent* Mesh{ ToyBallMachine->GetStaticMeshComponent() };
	UMaterialInterface* Material{ ToyBallMachine->GetStaticMeshComponent()->GetMaterial(DiscoMaterialIndex) };
	DynamicMatierial = UMaterialInstanceDynamic::Create(Material, this);
	Mesh->SetMaterial(DiscoMaterialIndex, DynamicMatierial);
}

// Called every frame
void ADiscoFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bMusicIsPlaying)
	{
		CurrentSecToChangeTexture += DeltaTime;

		if (CurrentSecToChangeTexture >= SecPerTextureChange)
		{
			CurrentSecToChangeTexture -= SecPerTextureChange;
			SwapToNextTexture();
		}
	}
}

void ADiscoFloor::SwapToNextTexture()
{	
	if (DynamicMatierial)
	{
		++TexureIndex;
		TexureIndex %= DiscoTextures.Num();
		UTexture* Texture{ DiscoTextures[TexureIndex] };

		if (!Texture)
			return;

		DynamicMatierial->SetTextureParameterValue(TEXT("Colors"), Texture);
	}
}

void ADiscoFloor::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bMusicIsPlaying)
		return;

	ADreamBotsCharacter* character = Cast<ADreamBotsCharacter>(OtherActor);

	if (character)
	{
		UAkGameplayStatics::SetState(State, StateGroupName, StateName);
		bMusicIsPlaying = true;
		CurrentSecToChangeTexture -= DelayBeat * SecPerBeat;
	}
}