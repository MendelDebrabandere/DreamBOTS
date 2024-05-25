// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckArea.h"
#include "DreamBotsCharacter.h"

// Sets default values
ACheckArea::ACheckArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SceneComponent->SetupAttachment(RootComponent);

	BoxCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollisionComponent->SetupAttachment(SceneComponent);
	BoxCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ACheckArea::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ACheckArea::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACheckArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACheckArea::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADreamBotsCharacter* PlayerCharacter = Cast<ADreamBotsCharacter>(OtherActor);

	if(PlayerCharacter)
	{
		PlayerCharacter->SetZThreshold(NewZThresholdValue);
	}
}

