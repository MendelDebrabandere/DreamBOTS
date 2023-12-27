// Fill out your copyright notice in the Description page of Project Settings.


#include "TalkingHead.h"

#include "DreamBotsCharacter.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkAudioEvent.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ATalkingHead::ATalkingHead()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	HeadPivot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadPivot"));
	HeadPivot->SetupAttachment(Root);

	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(HeadPivot);

	StartFollowBox = CreateDefaultSubobject<UBoxComponent>(TEXT("StartFollowBox"));
	StartFollowBox->SetupAttachment(Root);

	EndFollowBox = CreateDefaultSubobject<UBoxComponent>(TEXT("EndFollowBox"));
	EndFollowBox->SetupAttachment(Root);

	EndPoint = CreateDefaultSubobject<USceneComponent>(TEXT("EndPoint"));
	EndPoint->SetupAttachment(Root);

	HeadAudio = CreateDefaultSubobject<UAkComponent>(TEXT("HeadAudio"));
	HeadAudio->SetupAttachment(HeadMesh);


}

// Called when the game starts or when spawned
void ATalkingHead::BeginPlay()
{
	Super::BeginPlay();

	HeadPivot->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	HeadPivot->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);


	StartFollowBox->OnComponentBeginOverlap.AddDynamic(this, &ATalkingHead::OnBeginOverlap);
	EndFollowBox->OnComponentEndOverlap.AddDynamic(this, &ATalkingHead::OnEndOverlap);

	HeadAudio->SetRTPCValue(RtpcSpeed, 0.f, 0, RtpcSpeed->GetName());
	StartFly();

	// show scenehead
	if (SceneHeadMesh)
	{
		TSet<UActorComponent*> components = SceneHeadMesh->GetComponents();

		for (UActorComponent* Comp : components)
		{
			UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(Comp);
			if (MeshComp)
			{
				// Set the visibility to false
				MeshComp->SetVisibility(false);
			}
		}
	}
}

void ATalkingHead::MoveHeadTowardsPlayer(float DeltaTime)
{
	if (bShouldFollow && Character)
	{
		// MAKE HEAD FOLOW YOU
		FVector CharacterLocation = Character->GetActorLocation();
		FVector HeadMeshLocation = HeadPivot->GetComponentLocation();

		CharacterLocation.Z = HeadMeshLocation.Z;

		float Distance = FVector::Dist(CharacterLocation, HeadMeshLocation);

		FVector Direction = CharacterLocation - HeadMeshLocation;
		Direction.Z = 0;
		Direction.Normalize(); // Ensure it's a unit vector for direction

		if (Distance >= FollowDistance)
		{
			// make it move behind the player
			float ForceScale = 900000.0f;


			HeadPivot->AddForce(Direction * ForceScale);
		}

		//MAKE HEAD LOOK AT YOU

		// Creating a rotation that looks in the direction of the character
		FRotator TargetRotation = FRotationMatrix::MakeFromX(Direction).Rotator();

		// Current head rotation
		FRotator CurrentRotation = HeadPivot->GetComponentRotation();

		// Smoothly interpolate to the target rotation
		float InterpSpeed = 5.0f; // Adjust this value for the rotation speed
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, InterpSpeed);

		// Set the new rotation
		HeadPivot->SetWorldRotation(NewRotation);

	}
}

void ATalkingHead::SineWaveHeadBobbing(float dTime)
{
	if (CurrBobbingSpeed != 0.f)
	{
		SinePosition += CurrBobbingSpeed * dTime;

		HeadMesh->SetRelativeLocation(FVector{ 0, HorBobbing * sinf(SinePosition), VertBobbing * sinf(2 * SinePosition) });
	}
}

void ATalkingHead::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	ADreamBotsCharacter* character = Cast<ADreamBotsCharacter>(OtherActor);

	if (character)
	{
		Character = character;
		bShouldFollow = true;
		StartTalking();
		CurrBobbingSpeed = BobbingSpeed;
	}
}

void ATalkingHead::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ADreamBotsCharacter* character = Cast<ADreamBotsCharacter>(OtherActor);

	if (character)
	{
		bShouldFollow = false;

		HeadPivot->SetSimulatePhysics(false);

		EMoveComponentAction::Type MoveActionFlag = EMoveComponentAction::Move;
		FLatentActionInfo LatenInfo;
		LatenInfo.CallbackTarget = this;
		UKismetSystemLibrary::MoveComponentTo(HeadPivot, EndPoint->GetComponentLocation(), EndPoint->GetComponentRotation(), true, true, 4.f, true, MoveActionFlag, LatenInfo);
		GetWorldTimerManager().SetTimer(MuteTimerHandle, this, &ATalkingHead::StopTalking, 4.f, false);

		CurrBobbingSpeed = 0.f;
		SinePosition = 0.f;

		StartTime = GetWorld()->GetTimeSeconds();
		Duration = 4.f; // Set your duration here

		// Set up the timer to call ScaleMeshOverTime repeatedly
		GetWorldTimerManager().SetTimer(ScaleTimerHandle, this, &ATalkingHead::ScaleMeshOverTime, 0.02f, true);
	}
}

void ATalkingHead::ScaleMeshOverTime()
{
	float ElapsedTime = GetWorld()->GetTimeSeconds() - StartTime;
	if (ElapsedTime < Duration)
	{
		FVector CurrentScale = FVector(1.5f, 1.5f, 1.5f);
		FVector TargetScale = FVector(3.5f, 3.5f, 3.5f); // Set your target scale here
		float Alpha = ElapsedTime / Duration;
		FVector NewScale = FMath::Lerp(CurrentScale, TargetScale, Alpha);

		HeadMesh->SetWorldScale3D(NewScale);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(ScaleTimerHandle);

		HeadMesh->SetVisibility(false);

		// show scenehead
		if (SceneHeadMesh)
		{
			TSet<UActorComponent*> components = SceneHeadMesh->GetComponents();

			for (UActorComponent* Comp : components)
			{
				UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(Comp);
				if (MeshComp)
				{
					MeshComp->SetVisibility(true);
				}
			}
		}
	}
}


// Called every frame
void ATalkingHead::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Audio rtpc update
	const float value{ static_cast<float>(HeadPivot->GetComponentVelocity().Length()) / EngineMaxSpeed * 100.f };
	HeadAudio->SetRTPCValue(RtpcSpeed, value, 0, RtpcSpeed->GetName());

	MoveHeadTowardsPlayer(DeltaTime);
	SineWaveHeadBobbing(DeltaTime);
}

void ATalkingHead::StartTalking() const
{
	HeadAudio->PostAkEvent(WEStartTalking);
}

void ATalkingHead::StopTalking() const
{
	HeadAudio->PostAkEvent(WEStopTalking);
}

void ATalkingHead::StartFly() const
{
	HeadAudio->PostAkEvent(WEStartFlying);
}

