// Fill out your copyright notice in the Description page of Project Settings.


#include "MovieTape.h"
#include "DreamBotsCharacter.h"
// Sets default values
AMovieTape::AMovieTape()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionMesh = CreateDefaultSubobject<UBoxComponent>(FName("CollisionMesh"));	
	RootComponent = CollisionMesh;

	MovieTapeMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("MovieTapeMesh"));
	MovieTapeMesh->SetupAttachment(RootComponent);

	CollisionMesh->OnComponentBeginOverlap.AddDynamic(this, &AMovieTape::OnOverlapBegin);
	
}

// Called when the game starts or when spawned
void AMovieTape::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMovieTape::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADreamBotsCharacter* character = Cast<ADreamBotsCharacter>(OtherActor);
	if (character != nullptr)
	{
		character->AddMovieTape(MovieTapeEnum);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Overlapped!"));

		bCollected = true;

		CollisionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		GetWorldTimerManager().SetTimer(DestroyTimer, this, &AMovieTape::DestroyTape, 1.0f);
	}
}

// Called every frame
void AMovieTape::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateMovieTape(DeltaTime);

	if(!bCollected)
	{
		ScaleAnimation(DeltaTime);
	}
	else
	{
		FVector Scale = FMath::Lerp(GetActorScale3D(), FVector(0.0, 0.0, 0.0), DeltaTime * 3.f);

		SetActorScale3D(Scale);
	}
}

void AMovieTape::RotateMovieTape(float elapsed)
{
	TotalTime += elapsed;

	//Bobbing 
	float CalculateBobbingSpeed = TotalTime * BobbingSpeed;

	FVector bobbingDeltaOffset = { 0.f,0.f,0.f };
	bobbingDeltaOffset.X = FMath::Sin(CalculateBobbingSpeed) * HeightDisplacement;
	//AddActorLocalOffset(bobbingDeltaOffset);

	//Rotation
	AddActorLocalRotation(RotationDirection * elapsed);
}

void AMovieTape::ScaleAnimation(float elapsedSec)
{
	TimeAccumulator += elapsedSec;

	float Scale = Amplitude * FMath::Sin(Frequency * TimeAccumulator);

	Scale = FMath::Lerp(MinScale, MaxScale, Scale);

	SetActorScale3D(FVector(Scale));
}

void AMovieTape::DestroyTape()
{
	Destroy();
}
