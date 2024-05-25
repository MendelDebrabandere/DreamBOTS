// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeConsole.h"
#include "Kismet/GameplayStatics.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkAudioEvent.h"

// Sets default values
AMazeConsole::AMazeConsole()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create Root
	ToyballConsoleRoot = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	ToyballConsoleRoot->SetupAttachment(RootComponent);
	//Create CollisionMesh
	CollisionMesh = CreateDefaultSubobject<UBoxComponent>(FName("CollisionMesh"));
	CollisionMesh->SetupAttachment(ToyballConsoleRoot);

	//Create Drone Machine Static Mesh
	ToyballConsoleMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("ToyballeMesh"));
	ToyballConsoleMesh->SetupAttachment(ToyballConsoleRoot);

}

void AMazeConsole::ActivateToyBalls()
{
	if (IsToyballConsoleActive)
	{

		// Wwise 
		// Enter event
		FOnAkPostEventCallback nullCallback{};
		WEActivate->PostOnActor(this, nullCallback, 0, false);

		AToyBall* Toyball = nullptr;
		for (auto& actor : FoundActors)
		{
			Toyball = Cast<AToyBall>(actor);
			Toyball->MoveUpwards();
		}
	}
}

// Called when the game starts or when spawned
void AMazeConsole::BeginPlay()
{
	Super::BeginPlay();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ToyballClass, FoundActors);
}

// Called every frame
void AMazeConsole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

