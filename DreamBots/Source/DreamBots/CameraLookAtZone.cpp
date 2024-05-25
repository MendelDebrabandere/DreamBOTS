// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraLookAtZone.h"
#include "Components/BoxComponent.h"
#include "DreamBotsCharacter.h"
#include "DroneBase.h"

// Sets default values
ACameraLookAtZone::ACameraLookAtZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	RootComponent = BoxCollision;

    LookAtPoint = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    LookAtPoint->SetupAttachment(RootComponent);

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ACameraLookAtZone::OnBeginOverlap);
}
// Called when the game starts or when spawned
void ACameraLookAtZone::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACameraLookAtZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACameraLookAtZone::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && (OtherActor != this))
    {
        ADreamBotsCharacter* PlayerChar = Cast<ADreamBotsCharacter>(OtherActor);
        if (PlayerChar)
        {
            // Call your player character's functions here
            PlayerChar->SetCameraLookAtPoint(LookAtPoint, LookAtInFrontOfPlayer, BoomArmPitch, CameraPitch, CameraBoomLength, CameraTransitionSpeed);
        }
        else
        {
            ADroneBase* DroneChar = Cast<ADroneBase>(OtherActor);
            if (DroneChar)
            {
                DroneChar->SetCameraLookAtPoint(LookAtPoint, LookAtInFrontOfPlayer, BoomArmPitch, CameraPitch, CameraBoomLength, CameraTransitionSpeed);
            }
        }
    }
}