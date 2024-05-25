// Fill out your copyright notice in the Description page of Project Settings.


#include "ToyBall.h"
#include "DreamBotsCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkComponent.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkAudioEvent.h"

// Sets default values
AToyBall::AToyBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a ToyBall Mesh
	ToyBallMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ToyBallMesh"));
	ToyBallMeshComponent->SetupAttachment(RootComponent);

	// Create a Box Collision Component
	BoxCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollisionComponent->SetupAttachment(ToyBallMeshComponent);
	BoxCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AToyBall::OnOverlapBegin);

	GrabberComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Claw"));
	GrabberComponent->SetupAttachment(ToyBallMeshComponent);

	BallAudio = CreateDefaultSubobject<UAkComponent>(TEXT("BallAudio"));
	BallAudio->SetupAttachment(ToyBallMeshComponent);
}

// Called when the game starts or when spawned
void AToyBall::BeginPlay()
{
	Super::BeginPlay();
	
	GrabberActor = Cast<AGrabbingMachine>(GrabberComponent->GetChildActor());
	GrabberActor->SetActorEnableCollision(ECollisionEnabled::NoCollision);

	HideGrabber(true);
}

// Called every frame
void AToyBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bPickUpBall && TriggeredByPlayer)
	{
		GrabberComponent->SetWorldLocation(FMath::Lerp(GrabberComponent->GetComponentLocation(), ToyBallMeshComponent->GetComponentLocation() - FVector(0.0, 0.0, 400.0), DeltaTime * 5.f));

	/*	if (FVector::DistSquared(GrabberComponent->GetComponentLocation(), ToyBallMeshComponent->GetComponentLocation()) < FMath::Square(1.f))
		{
			bPickUpBall = false;

			UE_LOG(LogTemp, Warning, TEXT("Some warning message"));
		}*/
	}
}

void AToyBall::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the Overlapping Actor is the Player Character
	ADreamBotsCharacter* PlayerCharacter = Cast<ADreamBotsCharacter>(OtherActor);

	if(PlayerCharacter && TriggeredByPlayer)
	{
		GetWorldTimerManager().SetTimer(DelayMoveTimer, this, &AToyBall::MoveUpwards, 0.5f, false);
		bPickUpBall = true;
		USkeletalMeshComponent* SkeletalMeshComponent = GrabberActor->FindComponentByClass<USkeletalMeshComponent>();
		
		if(SkeletalMeshComponent)
		{
			HideGrabber(false);
		}

	}
}

void AToyBall::MoveUpwards()
{
	//Get the Root Component of the Player
	USceneComponent* RootComponentChar = GetRootComponent();

	//Save the Location the Carrot is supposed to move to
	FVector TargetLocation = RootComponent->GetComponentLocation() + FVector(0.0, 0.0, DistanceToMoveUp);

	EMoveComponentAction::Type MoveActionFlag = EMoveComponentAction::Move;
	FLatentActionInfo LatenInfo;
	LatenInfo.CallbackTarget = this;

	//Move ToyBall to Desired Location in 2 Seconds
	UKismetSystemLibrary::MoveComponentTo(RootComponentChar, TargetLocation, GetActorRotation(), false, false, 2.f, false, MoveActionFlag, LatenInfo);

	BallAudio->PostAkEvent(WERise);

	if (GrabberActor)
	{
		BallAudio->PostAkEvent(WEGrab);
		GrabberActor->SetCanGrabPlayer(false);
		GrabberActor->SetClosed(true);
	}
}

