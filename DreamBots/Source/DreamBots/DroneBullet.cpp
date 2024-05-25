// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneBullet.h"

// Sets default values
ADroneBullet::ADroneBullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DroneBulletRoot = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	RootComponent = DroneBulletRoot;

	CollisionMesh = CreateDefaultSubobject<USphereComponent>(FName("CollisionMesh"));
	CollisionMesh->SetupAttachment(RootComponent);
	//CollisionMesh->bHiddenInGame = false;


	ProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));
	ProjectileComponent->SetUpdatedComponent(DroneBulletRoot);
	ProjectileComponent->InitialSpeed = 1000.f;
	ProjectileComponent->MaxSpeed = 1500.f;
	ProjectileComponent->bRotationFollowsVelocity = true;
	ProjectileComponent->bShouldBounce = true;
	ProjectileComponent->Bounciness = 0.3f;
	ProjectileComponent->ProjectileGravityScale = 0.0f;

	SetLifeSpan(1.0);
}

void ADroneBullet::FireProjectile(FVector LaunchDirection)
{
	if (ProjectileComponent)
	{
		ProjectileComponent->Velocity = LaunchDirection * ProjectileComponent->InitialSpeed;
	}
}

void ADroneBullet::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Overlap!"));
	AWasp* wasp = Cast<AWasp>(OtherActor);
	if (wasp)
	{

		wasp->GetHit();
		Destroy();
	}

}

// Called when the game starts or when spawned
void ADroneBullet::BeginPlay()
{
	Super::BeginPlay();
	CollisionMesh->OnComponentBeginOverlap.AddDynamic(this, &ADroneBullet::OnOverlapBegin);

}

// Called every frame
void ADroneBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

