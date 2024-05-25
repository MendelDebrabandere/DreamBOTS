// Fill out your copyright notice in the Description page of Project Settings.


#include "CleaningWall.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ACleaningWall::ACleaningWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	WallMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WallMesh"));
	WallMesh->SetupAttachment(Root);


}

// Called when the game starts or when spawned
void ACleaningWall::BeginPlay()
{
	Super::BeginPlay();

	SpawnScale = WallMesh->GetRelativeScale3D();

	TargetScale = SpawnScale;
	CurrentScale = TargetScale;
}

// Called every frame
void ACleaningWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Interpolate from CurrentScale to TargetScale
	FVector NewScale = FMath::Lerp(CurrentScale, TargetScale, DeltaTime * 5.f);

	// Set the new scale
	WallMesh->SetRelativeScale3D(NewScale);

	// Update CurrentScale for the next frame
	CurrentScale = NewScale;


}

void ACleaningWall::SetBlackPercentage(int percentage, int destroyPercentage)
{
	TargetScale = FVector{ SpawnScale.X,SpawnScale.Y, SpawnScale.Z * (percentage / 100.f) };

	if (percentage <= destroyPercentage)
	{
		TargetScale = FVector{ SpawnScale.X,SpawnScale.Y, 0};

		FTimerHandle MyTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(MyTimerHandle, this, &ACleaningWall::DestroyObject, 0.5f, false);
	}

}

