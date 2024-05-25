// Fill out your copyright notice in the Description page of Project Settings.

#include "ShootingDrone.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputSubsystems.h"
#include "Math/Vector.h"
#include "Kismet/KismetMathLibrary.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkAudioEvent.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkComponent.h"

AShootingDrone::AShootingDrone()
{
	NiagaraComponentLeft = CreateDefaultSubobject<UNiagaraComponent>(FName("NiagaraComponentLeft"));
	NiagaraComponentLeft->SetupAttachment(DroneAttachment);

	NiagaraComponentRight = CreateDefaultSubobject<UNiagaraComponent>(FName("NiagaraComponentRight"));
	NiagaraComponentRight->SetupAttachment(DroneAttachment);


	// Create AkComponent and attach it to the SceneRoot
	PoisonAudio = CreateDefaultSubobject<UAkComponent>(TEXT("PoisonAudio"));
	PoisonAudio->SetupAttachment(DroneAttachment);
}

void AShootingDrone::ShootProjectile()
{
	if (ProjectileClass)
	{

		if (CurrentTiming <= 0)
		{
			ADroneBullet* ProjectileLeft = GetWorld()->SpawnActor<ADroneBullet>(ProjectileClass, DroneMesh->GetComponentTransform());
			CurrentTiming = MaxTiming;
		}

		CheckWasps();
	}

}

void AShootingDrone::StartShootProjectileNiagara()
{
	if (NiagaraSystem)
	{

		NiagaraComponentLeft->Activate();
		NiagaraComponentRight->Activate();

	}

	PoisonAudio->PostAkEvent(WEShoot);
}

void AShootingDrone::StopShootingProjectile()
{
	NiagaraComponentLeft->Deactivate();
	NiagaraComponentRight->Deactivate();

	PoisonAudio->PostAkEvent(WEStopShoot);
}

void AShootingDrone::ReturnToPlayer()
{
	if (!HasSpawnMovieTapeOnce)
	{
		HasSpawnMovieTapeOnce = true;
		ReturnToInitialSpot();
	}
	StopShootingProjectile();
	DroneMachine->ChangePossession(Cast<APlayerController>(Controller));
	DroneMachine->DisableMachine();
}

void AShootingDrone::CheckWasps()
{

	if (AWasp::GetWaspCounter() <= 0)
	{
		ResetDroneSpeedRPTC();
		ReturnToPlayer();
	}
}

void AShootingDrone::AutoAim(float deltaTime)
{
	IsWaspChecked = false;
	for (int i = 0; i < FoundActors.Num(); i++)
	{
		float distance = FVector::Distance(FoundActors[i]->GetActorLocation(), GetActorLocation());
		if (distance < ClosestWasp)
		{
			ClosestWasp = distance;
			ClosestWaspIdx = i;
			break;
		}
	}
	if (ClosestWaspIdx >= FoundActors.Num())
	{
		return;
	}
	ClosestWasp = FVector::Distance(FoundActors[ClosestWaspIdx]->GetActorLocation(), GetActorLocation());
	auto rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), FoundActors[ClosestWaspIdx]->GetActorLocation());
	auto interpRotation = UKismetMathLibrary::RInterpTo(DroneMesh->GetComponentRotation(), rotation, deltaTime, InterpSpeed);
	DroneMesh->SetWorldRotation(interpRotation);
	//SetActorRotation(interpRotation);
}

void AShootingDrone::BeginPlay()
{
	Super::BeginPlay();
	NiagaraComponentLeft->SetAsset(NiagaraSystem);
	NiagaraComponentRight->SetAsset(NiagaraSystem);


}

void AShootingDrone::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CurrentTiming -= DeltaSeconds;
	if (!IsWaspChecked)
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), WaspClass, FoundActors);
		IsWaspChecked = true;
	}
	if (AWasp::GetWaspCounter() > 0)
	{
		AutoAim(DeltaSeconds);
	}
}
void AShootingDrone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADroneBase::Move);

		//Shooting
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AShootingDrone::ShootProjectile);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AShootingDrone::StartShootProjectileNiagara);

		//End shooting
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &AShootingDrone::StopShootingProjectile);
	}
}

