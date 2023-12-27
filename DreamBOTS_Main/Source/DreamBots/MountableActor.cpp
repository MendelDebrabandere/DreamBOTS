// Fill out your copyright notice in the Description page of Project Settings.

#include "MountableActor.h"
#include "NiagaraComponent.h"
#include "DreamBotsCharacter.h"

// Sets default values
AMountableActor::AMountableActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	Root->SetupAttachment(RootComponent);

	RideRoot = CreateDefaultSubobject<USceneComponent>(FName("RidingRoot"));
	RideRoot->SetupAttachment(Root);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(FName("CollisionMesh"));
	CollisionBox->SetupAttachment(Root);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(Root);

	FireParticles = CreateDefaultSubobject<UNiagaraComponent>(FName("Fire"));
	FireParticles->SetupAttachment(SkeletalMesh);
}

// Called when the game starts or when spawned
void AMountableActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMountableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

