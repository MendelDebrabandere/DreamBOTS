// Fill out your copyright notice in the Description page of Project Settings.


#include "ClothingMachine.h"

// Sets default values
AClothingMachine::AClothingMachine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create and attach the SceneRoot
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	// Create the StaticMeshComponent and attach it to the SceneRoot
	Gear = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gear"));
	Gear->SetupAttachment(RootComponent);

	// Create the StaticMeshComponent and attach it to the SceneRoot
	GearColours = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GearColours"));
	GearColours->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AClothingMachine::BeginPlay()
{
	Super::BeginPlay();
	
}

void AClothingMachine::HandleRotation(float DeltaTime)
{
	const FRotator deltaRotation{ 0.f, RotationSpeed * DeltaTime, 0.f };
	AddActorLocalRotation(deltaRotation);
}

// Called every frame
void AClothingMachine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HandleRotation(DeltaTime);
}

