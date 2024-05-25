// Fill out your copyright notice in the Description page of Project Settings.


#include "LegoDrone.h"
#include "LegoBridge.h"
#include "LegoPiece.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkAudioEvent.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkComponent.h"
#include "Kismet/KismetMathLibrary.h"

ALegoDrone::ALegoDrone()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PickUpBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("PickUpBox"));
	PickUpBoxComponent->SetupAttachment(DroneRoot);
	PickUpBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ALegoDrone::OnOverlapBegin);

	// Component that handles magnet audio
	MagnetAudio = CreateDefaultSubobject<UAkComponent>(TEXT("MagnetAudio"));
	MagnetAudio->SetupAttachment(DroneRoot);

	ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowMesh"));
	ArrowMesh->SetupAttachment(DroneMesh);
}

void ALegoDrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(PiecesPlaced >= 5 && DroneMachine)
	{
		APlayerController* PlayerCharacterController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

		ResetDroneSpeedRPTC();
		ReturnToInitialSpot();
		DroneMachine->ChangePossession(PlayerCharacterController);
		DroneMachine->DisableMachine();
		PiecesPlaced = 0;
	}

	if(!LegoPieces.IsEmpty() && IsPlayerControlled())
	{
		if (!bAttached)
		{
			float ClosestDistance = MAX_FLT;

			for (ALegoPiece* Actor : LegoPieces)
			{
				if (!Actor)
					continue;

				float Distance = (Actor->GetActorLocation() - GetActorLocation()).Size();

				if (Distance < ClosestDistance)
				{
					if (ClosestPiece)
					{
						ClosestPiece->GetLegoPieceMesh()->SetRenderCustomDepth(false);
					}

					ClosestPiece = Actor;
					ClosestDistance = Distance;
				}
			}

			TargetLocation = ClosestPiece->GetActorLocation();

			ClosestPiece->GetLegoPieceMesh()->SetRenderCustomDepth(true);
			ClosestPiece->GetLegoPieceMesh()->CustomDepthStencilValue = 3;
		}
		else
		{
			TargetLocation = ClosestPiece->GetLegoBridge()->GetActorLocation();
			ClosestPiece->GetLegoPieceMesh()->SetRenderCustomDepth(false);
		}

		FVector Location = GetActorLocation();

		FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);

		float time = GetWorld()->GetDeltaSeconds();
		DroneMesh->SetWorldRotation(FMath::Lerp(DroneMesh->GetComponentRotation(), FRotator(0.0, NewRotation.Yaw, NewRotation.Roll), LerpValue * time * 80.f));
		ArrowMesh->SetWorldRotation(FMath::Lerp(ArrowMesh->GetComponentRotation(), NewRotation, LerpValue * time * 80.f));

		float CurrentTime = GetGameTimeSinceCreation();
		float Offset = 20.0f * FMath::Sin(CurrentTime * 10.0f);

		FVector NewLocation = DroneMesh->GetComponentLocation() + (ArrowMesh->GetForwardVector() * Offset) + DroneMesh->GetForwardVector() * 100.0;

		ArrowMesh->SetWorldLocation(NewLocation);
	}

}

void ALegoDrone::SetLegoAttach(bool bAttach)
{
	bAttached = bAttach;

	LegoPieces.Remove(ClosestPiece);

	// Wwise
	// Stop pickup loop
	MagnetAudio->PostAkEvent(WELegoReleased);
}

bool ALegoDrone::AttachStatus()
{
	return bAttached;
}

void ALegoDrone::BeginPlay()
{
	Super::BeginPlay();
}

void ALegoDrone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADroneBase::Move);
	}
}

void ALegoDrone::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(ALegoPiece* LegoPiece = Cast<ALegoPiece>(OtherActor))
	{
		if(!bAttached)
		{
			//LegoPiece->AttachToComponent(PickUpBoxComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false));
			//LegoPiece->SetActorLocation(FVector(LegoPiece->GetActorLocation().X, LegoPiece->GetActorLocation().Y, LegoPiece->GetActorLocation().Z - ZOffset));
			LegoPiece->MakeCorrespondingPieceVisible();
			LegoPiece->SetIsAttached(true);
			LegoPiece->SetLDrone(this);

			bAttached = true;

			// Wwise
			// Start pickup loop
			MagnetAudio->PostAkEvent(WELegoPickedup);
		}
	}
}