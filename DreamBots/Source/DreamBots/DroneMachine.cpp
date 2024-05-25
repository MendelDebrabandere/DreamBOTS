// Fill out your copyright notice in the Description page of Project Settings.

#include "DroneMachine.h"
#include "DroneBase.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkAudioEvent.h"

// Sets default values
ADroneMachine::ADroneMachine()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create Root
	DroneMachineRoot = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	DroneMachineRoot->SetupAttachment(RootComponent);
	//Create CollisionMesh
	CollisionMesh = CreateDefaultSubobject<UBoxComponent>(FName("CollisionMesh"));
	CollisionMesh->SetupAttachment(DroneMachineRoot);

	//Create Drone Machine Static Mesh
	DroneMachineMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("DroneMachineMesh"));
	DroneMachineMesh->SetupAttachment(DroneMachineRoot);

}



// Called when the game starts or when spawned
void ADroneMachine::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ADroneMachine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsReturningToPlayerPossess)
	{
		MaxDelayToReturn -= DeltaTime;
		IsReturningToPlayerPossess = false;
	}
}

void ADroneMachine::ChangePossession(APlayerController* controller)
{
	if (DroneToPossess && IsDroneMachineActive)
	{
		if (CurrentPossess == Player)
		{
			controller->UnPossess();
			controller->Possess(DroneToPossess);
			DroneToPossess->SetDroneMachine(this);
			CurrentPossess = Drone;

			// Wwise 
			// Enter event
			FOnAkPostEventCallback nullCallback{};
			WEEnterDrone->PostOnActor(this, nullCallback, 0, false);
		}
		else if (CurrentPossess == Drone)
		{
			IsReturningToPlayerPossess = true;
			FTimerHandle TimerHandler;
			FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this,&ADroneMachine::RepossessToPlayer,controller);
			GetWorld()->GetTimerManager().SetTimer(TimerHandler, TimerDelegate, MaxDelayToReturn, false);
		
		}
	}
}

void ADroneMachine::RepossessToPlayer(APlayerController* controller)
{
	PlayerToPossess->ReturnPlayer();
	controller->UnPossess();
	controller->Possess(PlayerToPossess);
	CurrentPossess = Player;

	// Wwise
	// Exit event
	FOnAkPostEventCallback nullCallback{};
	WEExitDrone->PostOnActor(this, nullCallback, 0, false);
}

