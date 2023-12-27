// Fill out your copyright notice in the Description page of Project Settings.


#include "Cannonball.h"
#include "GameFramework/PawnMovementComponent.h"
#include "DreamBotsCharacter.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkComponent.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkAudioEvent.h"

// Sets default values
ACannonball::ACannonball()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	// Create AkComponent and attach it to the SceneRoot
	BallAudio = CreateDefaultSubobject<UAkComponent>(TEXT("BallAudio"));
	BallAudio->SetupAttachment(RootComponent);

	// Enable Physics
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetNotifyRigidBodyCollision(true);
	Mesh->OnComponentHit.AddDynamic(this, &ACannonball::OnHit);

	CurrLifeTime = MaxLifeTime;
}

// Called when the game starts or when spawned
void ACannonball::BeginPlay()
{
	Super::BeginPlay();
	newCannonBallSize = GetActorScale().X;

}

void ACannonball::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Check if the Overlapping Actor is the Player Character
	ADreamBotsCharacter* PlayerCharacter = Cast<ADreamBotsCharacter>(OtherActor);

	FVector velocity = GetVelocity();
	Mesh->SetPhysicsLinearVelocity(velocity);


	if (PlayerCharacter)
	{
		NormalImpulse.Z = 0;
		NormalImpulse *= -1;
		NormalImpulse.Normalize();
		NormalImpulse.Z = VerticalKnockBack;
		PlayerCharacter->LaunchCharacter(NormalImpulse * KnockBackForce, true, true);

		BallAudio->PostAkEvent(WEPlayerImpact);
	}
	else
	{
		double impulseLength{ NormalImpulse.Length() }; // Change to lengthSquared

		if (impulseLength >= PhysicsAudioTreshhold) // Trun them into squared in constructor
		{
			impulseLength = std::clamp(impulseLength, PhysicsAudioTreshhold, PhysicsAudioMax);
			const float value{ static_cast<float>((impulseLength - PhysicsAudioTreshhold) / (PhysicsAudioMax - PhysicsAudioTreshhold) * 100.f) };
			BallAudio->SetRTPCValue(RTPCVelocity, value, 0, RTPCVelocity->GetName());
			BallAudio->PostAkEvent(WEPhysicImpact);
			//UE_LOG(LogTemp, Warning, TEXT("velocity: %f"), value);
		}
	}
}

// Called every frame
void ACannonball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrLifeTime -= DeltaTime;
	
	if (CurrLifeTime <= 1.f)
	{
		newCannonBallSize -= DeltaTime;
		SetActorScale3D(FVector{ newCannonBallSize,newCannonBallSize,newCannonBallSize });
	}
	if (CurrLifeTime <= 0.f)
	{
		Destroy();
	}
}

void ACannonball::Shoot(FVector Direction, float Force)
{
	if (Mesh)
	{
		// Ensure the direction is normalized
		Direction.Normalize();

		// Apply the impulse
		Mesh->AddImpulse(Direction * Force);

		// Wwise
		// Post tail event
		BallAudio->PostAkEvent(WETail);
	}
}