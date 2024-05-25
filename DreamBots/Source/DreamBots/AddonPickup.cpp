// Fill out your copyright notice in the Description page of Project Settings.


#include "AddonPickup.h"
#include "Components/BoxComponent.h"

// Sets default values
AAddonPickup::AAddonPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	RootComponent = Root;

	CollisionMesh = CreateDefaultSubobject<UBoxComponent>(FName("CollisionMesh"));
	CollisionMesh->SetupAttachment(RootComponent);

	AddonMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("AddonMesh"));
	AddonMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AAddonPickup::BeginPlay()
{
	Super::BeginPlay();

	CollisionMesh->OnComponentBeginOverlap.AddDynamic(this, &AAddonPickup::OnOverlapBegin);

}

// Called every frame
void AAddonPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RotatePickup(DeltaTime);
}

void AAddonPickup::RotatePickup(float elapsed)
{
	//Rotation
	AddActorLocalRotation(RotationDirection * elapsed * RotationSpeed);
}

void AAddonPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADreamBotsCharacter* character = Cast<ADreamBotsCharacter>(OtherActor);
	if (character != nullptr)
	{
		character->ChangeCharacterType(Type, true, !ShouldDisappear);

		if (ShouldDisappear)
			Destroy();
	}
}
