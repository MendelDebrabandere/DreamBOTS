// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraCollisionChanger.h"
#include "Components/BoxComponent.h"
#include "DreamBotsCharacter.h"

// Sets default values
ACameraCollisionChanger::ACameraCollisionChanger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	RootComponent = BoxCollision;

}

// Called when the game starts or when spawned
void ACameraCollisionChanger::BeginPlay()
{
	Super::BeginPlay();

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ACameraCollisionChanger::OnBeginOverlap);

}

// Called every frame
void ACameraCollisionChanger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ACameraCollisionChanger::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && (OtherActor != this))
    {
        ADreamBotsCharacter* PlayerChar = Cast<ADreamBotsCharacter>(OtherActor);
        if (PlayerChar)
        {
            // Call your player character's functions here
            PlayerChar->SetCameraCollision(bNewCollisionValue);
        }
    }
}