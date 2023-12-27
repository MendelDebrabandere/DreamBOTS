// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorSwappingZone.h"
#include "Components/BoxComponent.h"

// Sets default values
AColorSwappingZone::AColorSwappingZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	Root->SetupAttachment(RootComponent);

	BoxComp = CreateDefaultSubobject<UBoxComponent>(FName("Box"));
	BoxComp->SetupAttachment(Root);
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AColorSwappingZone::OnBeginOverlap);

}

// Called when the game starts or when spawned
void AColorSwappingZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AColorSwappingZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AColorSwappingZone::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the Overlapping Actor is the Player Character
	ADreamBotsCharacter* PlayerCharacter = Cast<ADreamBotsCharacter>(OtherActor);

	if (PlayerCharacter)
	{
		PlayerCharacter->ChangeCharacterColor(NewColor, true);
	}
}

