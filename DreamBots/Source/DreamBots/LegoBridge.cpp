// Fill out your copyright notice in the Description page of Project Settings.


#include "LegoBridge.h"
#include "LegoDrone.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkAudioEvent.h"

// Sets default values
ALegoBridge::ALegoBridge()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SceneComponent->SetupAttachment(RootComponent);

	LegoPieceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LegoPiece"));
	LegoPieceMesh->SetupAttachment(SceneComponent);

	PlaceLegoPieceBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PlaceLegoPieceBox"));
	PlaceLegoPieceBox->SetupAttachment(LegoPieceMesh);

}

// Called when the game starts or when spawned
void ALegoBridge::BeginPlay()
{
	Super::BeginPlay();

	if(MaterialsBridge.Num() == LegoPieceMesh->GetNumMaterials())
	{
		for(int i{0}; i < MaterialsBridge.Num(); ++i)
		{
			DynamicMaterialsBridge.Add(UMaterialInstanceDynamic::Create(MaterialsBridge[i], this));
		}
	}

	LegoPieceMesh->SetHiddenInGame(true);

	PlaceLegoPieceBox->OnComponentBeginOverlap.AddDynamic(this, &ALegoBridge::OnOverlapBegin);

	PlaceLegoPieceBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LegoPieceMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called every frame
void ALegoBridge::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALegoBridge::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(ALegoDrone* LegoDrone = Cast<ALegoDrone>(OtherActor))
	{
		TArray<AActor*> AttachedActors;
		LegoDrone->GetAttachedActors(AttachedActors);

		for (AActor* AttachedActor : AttachedActors)
		{
			if (AttachedActor->ActorHasTag(FName("Lego")))
			{
				LegoDrone->SetLegoAttach(false);
				LegoDrone->IncreasePiecesPlaced();

				PlaceLegoPieceBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				if((MaterialsBridge.Num() == LegoPieceMesh->GetNumMaterials()))
				{
					for (int i{ 0 }; i < MaterialsBridge.Num(); ++i)
					{
						LegoPieceMesh->SetMaterial(i, DynamicMaterialsBridge[i]);
					}

				}

				//turn on piece collision
				LegoPieceMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

				AttachedActor->Destroy();

				FOnAkPostEventCallback nullCallback{};
				WELegoPlaced->PostOnActor(LegoPieceMesh->GetOwner(), nullCallback, 0, false);
			}
		}
	}
}

