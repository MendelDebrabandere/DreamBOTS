// Fill out your copyright notice in the Description page of Project Settings.


#include "BallShootingMachine.h"
#include "NiagaraComponent.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkComponent.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkAudioEvent.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkStateValue.h"
#include "Components/BoxComponent.h"
#include "DreamBotsCharacter.h"

// Sets default values
ABallShootingMachine::ABallShootingMachine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create and attach the SceneRoot
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

#pragma region Creating SM
	// Create the StaticMeshComponent and attach it to the SceneRoot
	Tower1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tower1"));
	Tower1->SetupAttachment(RootComponent);

	// Create the StaticMeshComponent and attach it to the SceneRoot
	Tower2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tower2"));
	Tower2->SetupAttachment(RootComponent);

	// Create the StaticMeshComponent and attach it to the SceneRoot
	Tower1Cog = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tower1Cog"));
	Tower1Cog->SetupAttachment(Tower1);
	Tower1Cog->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create the StaticMeshComponent and attach it to the SceneRoot
	Tower2Cog = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tower2Cog"));
	Tower2Cog->SetupAttachment(Tower2);
	Tower2Cog->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create the StaticMeshComponent and attach it to the SceneRoot
	Tower1Cannon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tower1Cannon"));
	Tower1Cannon->SetupAttachment(Tower1Cog);
	Tower1Cannon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create the StaticMeshComponent and attach it to the SceneRoot
	Tower2Cannon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tower2Cannon"));
	Tower2Cannon->SetupAttachment(Tower2Cog);
	Tower2Cannon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create the StaticMeshComponent and attach it to the SceneRoot
	Crystal1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Crystal1"));
	Crystal1->SetupAttachment(RootComponent);
	Crystal1->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create the StaticMeshComponent and attach it to the SceneRoot
	Crystal2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Crystal2"));
	Crystal2->SetupAttachment(RootComponent);
	Crystal2->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create the StaticMeshComponent and attach it to the SceneRoot
	Crystal1Cannon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Crystal1Cannon"));
	Crystal1Cannon->SetupAttachment(Crystal1);
	Crystal1Cannon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create the StaticMeshComponent and attach it to the SceneRoot
	Crystal2Cannon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Crystal2Cannon"));
	Crystal2Cannon->SetupAttachment(Crystal2);
	Crystal2Cannon->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	// CharacterSwapSmoke
	SmokeTower1Component = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SmokeTower1Component"));
	SmokeTower1Component->SetupAttachment(Tower1Cannon);
	SmokeTower1Component->bAutoActivate = false;

	SmokeTower2Component = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SmokeTower2Component"));
	SmokeTower2Component->SetupAttachment(Tower2Cannon);
	SmokeTower2Component->bAutoActivate = false;

	SmokeCrystal1Component = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SmokeCrystal1Component"));
	SmokeCrystal1Component->SetupAttachment(Crystal1Cannon);
	SmokeCrystal1Component->bAutoActivate = false;

	SmokeCrystal2Component = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SmokeCrystal2Component"));
	SmokeCrystal2Component->SetupAttachment(Crystal2Cannon);
	SmokeCrystal2Component->bAutoActivate = false;

	EffectsArray[0] = SmokeTower1Component;
	EffectsArray[1] = SmokeTower2Component;
	EffectsArray[2] = SmokeCrystal1Component;
	EffectsArray[3] = SmokeCrystal2Component;

#pragma endregion

	// Create AkComponent and attach it to the SceneRoot
	MovingAudio = CreateDefaultSubobject<UAkComponent>(TEXT("MovingAudio"));
	MovingAudio->SetupAttachment(RootComponent);

	CannonMeshes[0] = Tower1Cannon;
	CannonMeshes[1] = Tower2Cannon;
	CannonMeshes[2] = Crystal1Cannon;
	CannonMeshes[3] = Crystal2Cannon;

	// Create the ShootingZone BoxComponent and attach it to the SceneRoot
	ShootingZone = CreateDefaultSubobject<UBoxComponent>(TEXT("ShootingZone"));
	ShootingZone->SetupAttachment(RootComponent);
	ShootingZone->OnComponentBeginOverlap.AddDynamic(this, &ABallShootingMachine::OnBeginOverlapForShootingZone);
	ShootingZone->OnComponentEndOverlap.AddDynamic(this, &ABallShootingMachine::OnEndOverlapForShootingZone);
}

// Called when the game starts or when spawned
void ABallShootingMachine::BeginPlay()
{
	Super::BeginPlay();

	// Calculate seconds per beat
	SecPerBeat = 60.0 / BPM;
	TimeToRotate = SecPerBeat * 4;
	ShotDelay = SecPerBeat * 4;

	//set schooting timings
	for (int idx{}; idx < NrOfCannons; ++idx)
	{
		ShootingCooldownsArr[idx] = rand() % (MaxShootingCooldown - MinShootingCooldown) + MinShootingCooldown;
	}

	for (int idx{}; idx < NrOfCannons; ++idx)
	{
		GearSpawnRotations[idx] = CannonMeshes[idx]->GetAttachParent()->GetComponentRotation();
	}

	PrimaryActorTick.bCanEverTick = false;

}

void ABallShootingMachine::ShootingCooldown(float DeltaTime)
{
	CurrTimeToRotate += DeltaTime;

	if (CurrTimeToRotate >= TimeToRotate)
	{
		CurrTimeToRotate -= TimeToRotate;
		RotateCanons();
	}
}

void ABallShootingMachine::ShootBall(int index)
{
	if (CannonballClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector SpawnLocation = CannonMeshes[index]->GetComponentLocation();
		FRotator ShootRotation = CannonMeshes[index]->GetComponentRotation();

		
		// Randomly select one of the balls
		TSubclassOf<ACannonball> SelectedClass;
		int32 RandomChoice = FMath::RandRange(0, 2); // Generates a random number between 0 and 1

		switch (RandomChoice)
		{
		case 0:
			SelectedClass = CannonballClass;
			break;
		case 1:
			SelectedClass = TennisballClass;
			break;
		case 2:
			SelectedClass = HockeyballClass;
			break;
		default:
			SelectedClass = CannonballClass; // Default case, can also handle unexpected values
			break;
		}

		// Now spawn the actor with the randomly selected ball
		ACannonball* SpawnedActor = GetWorld()->SpawnActor<ACannonball>(SelectedClass, SpawnLocation, FRotator{}, SpawnParams);
		
		if (SpawnedActor)
		{
			FVector ForwardVector = ShootRotation.RotateVector(FVector{ 1,0,0 });
			ForwardVector.Normalize();
			SpawnedActor->Shoot(ForwardVector, ShootingForce);
			//UE_LOG(LogTemp, Warning, TEXT("ForwardVector: %s"), *ForwardVector.ToString());

			// Wwise
			// Play shooting audio
			FOnAkPostEventCallback nullCallback;
			WEShoot->PostAtLocation(SpawnLocation, ShootRotation, nullCallback, 0, this);
		}

		EffectsArray[index]->ActivateSystem();


	}
}

void ABallShootingMachine::RotateCanons()
{
	//ROTATE ALL CANONS
	//UE_LOG(LogTemp, Warning, TEXT("ROTATING ALL CANONS!!!"));

	CurrRotation += RotationIncrement * RotationDirection;

	//Change rotation direction if at edge
	if (abs(CurrRotation) >= MaxRotationIncrement)
		RotationDirection *= -1;

	for (int idx{}; idx < NrOfCannons; ++idx)
	{
		int MyYawRotation;

		if (idx % 2 == 1) //bottom 2
			MyYawRotation = -CurrRotation;
		else // top 2
			MyYawRotation = CurrRotation;


		TargetCannonRotations[idx] = GearSpawnRotations[idx];
		TargetCannonRotations[idx].Yaw += MyYawRotation;
	}

	// This will call `LerpCannonRotations` every frame.
	GetWorld()->GetTimerManager().SetTimer(LerpTimerHandle, this, &ABallShootingMachine::LerpCannonRotations, GetWorld()->GetDeltaSeconds(), true);

	////UPDATE SHOOTING TIMINGS
	//FTimerHandle TimerHandle;
	//GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABallShootingMachine::UpdateShootTimings, SecPerBeat * 2.0, false);

	// Wwise
	// Play moving audio
	FOnAkPostEventCallback nullCallback;
	MovingAudio->PostAssociatedAkEvent(0, nullCallback);
}

void ABallShootingMachine::UpdateShootTimings()
{
	for (int idx{}; idx < NrOfCannons; ++idx)
	{
		--ShootingCooldownsArr[idx];
		if (ShootingCooldownsArr[idx] <= 0)
		{
			//Reset timing
			ShootingCooldownsArr[idx] = rand() % (MaxShootingCooldown - MinShootingCooldown) + MinShootingCooldown;
			//Shoot a ball
			ShootBall(idx);
		}
	}
}


// Called every frame
void ABallShootingMachine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bPlayerIsInZone)
	{
		ShootingCooldown(DeltaTime);

		CurShotDelay += DeltaTime;

		if (CurShotDelay >= ShotDelay)
		{
			CurShotDelay -= ShotDelay;
			UpdateShootTimings();
		}
	}
}

void ABallShootingMachine::LerpCannonRotations()
{
	bool bAllCannonsReachedTarget = true;

	for (int idx = 0; idx < NrOfCannons; ++idx)
	{
		FRotator CurrentRotation = CannonMeshes[idx]->GetAttachParent()->GetComponentRotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetCannonRotations[idx], GetWorld()->GetDeltaSeconds(), LerpSpeed);

		CannonMeshes[idx]->GetAttachParent()->SetWorldRotation(NewRotation);

		// Check if we've nearly reached the target. The tolerance value can be adjusted as needed.
		if (!NewRotation.Equals(TargetCannonRotations[idx], 0.1f))
		{
			bAllCannonsReachedTarget = false;
		}
	}

	// If all cannons have reached their target rotations, clear the timer.
	if (bAllCannonsReachedTarget)
	{
		GetWorld()->GetTimerManager().ClearTimer(LerpTimerHandle);
	}
}

void ABallShootingMachine::OnBeginOverlapForShootingZone(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the Overlapping Actor is the Player Character
	ADreamBotsCharacter* PlayerCharacter = Cast<ADreamBotsCharacter>(OtherActor);

	if (PlayerCharacter)
	{
		if (bPlayerIsInZone)
			return;

		bPlayerIsInZone = true;
		CurrTimeToRotate = (SecPerBeat * 2) + FirstBeatOffset;
		CurShotDelay = 0.0;
		PrimaryActorTick.bCanEverTick = true;

		ShootBall(0);

		UAkGameplayStatics::SetState(MusicState, "Music", "Shooting");
	}
}

void ABallShootingMachine::OnEndOverlapForShootingZone(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Check if the Overlapping Actor is the Player Character
	ADreamBotsCharacter* PlayerCharacter = Cast<ADreamBotsCharacter>(OtherActor);

	if (PlayerCharacter)
	{
		bPlayerIsInZone = false;
	}
}