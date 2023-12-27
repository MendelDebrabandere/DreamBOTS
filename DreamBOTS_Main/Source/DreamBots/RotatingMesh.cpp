// Fill out your copyright notice in the Description page of Project Settings.


#include "RotatingMesh.h"

// Sets default values
ARotatingMesh::ARotatingMesh()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ARotatingMesh::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARotatingMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Root->AddLocalRotation(FRotator{ 0,360 * RotationsPerSecond * DeltaTime, 0 });
}

