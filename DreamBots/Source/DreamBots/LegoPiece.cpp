// Fill out your copyright notice in the Description page of Project Settings.


#include "LegoPiece.h"
#include "LegoBridge.h"
#include "LegoDrone.h"

// Sets default values
ALegoPiece::ALegoPiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SceneComponent->SetupAttachment(RootComponent);

	LegoPieceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LegoPiece"));
	LegoPieceMesh->SetupAttachment(SceneComponent);

	PickUpLegoBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PickUpLego"));
	PickUpLegoBox->SetupAttachment(LegoPieceMesh);

}

void ALegoPiece::SetLDrone(ALegoDrone* LDrone)
{
	LegoDrone = LDrone;
}

// Called when the game starts or when spawned
void ALegoPiece::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALegoPiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bIsAttached && LegoDrone)
	{
		SetActorLocation(FMath::Lerp(GetActorLocation(), LegoDrone->GetActorLocation(), DeltaTime * 1.5f));

		if(FVector::DistSquared(GetActorLocation(), LegoDrone->GetActorLocation()) < FMath::Square(600.f))
		{
			bIsAttached = false;
			AttachToComponent(LegoDrone->GetPBoxComponent(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false));
			//SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z));
		}

	}

}

void ALegoPiece::MakeCorrespondingPieceVisible()
{
	if(LegoBridge)
	{
		LegoBridge->GetCurrentLegoMesh()->SetHiddenInGame(false);
		LegoBridge->GetPlaceLegoPieceBox()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

