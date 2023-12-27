// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneBase.h"

#include "CameraLookAtZone.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkAudioEvent.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkComponent.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
ADroneBase::ADroneBase()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DroneRoot = CreateDefaultSubobject<UBoxComponent>(FName("Root"));
	DroneRoot->SetupAttachment(RootComponent);

	CollisionMesh = CreateDefaultSubobject<UBoxComponent>(FName("CollisionMesh"));
	CollisionMesh->SetupAttachment(DroneRoot);

	DroneMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("DroneMesh"));
	DroneMesh->SetupAttachment(DroneRoot);

	DroneAttachment = CreateDefaultSubobject<UStaticMeshComponent>(FName("DroneAttachment"));
	DroneAttachment->SetupAttachment(DroneMesh);

	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(FName("Movement"));

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(DroneRoot);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraRotationLagSpeed = 3.f;

	StartingRotation = GetActorRotation();

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create AkComponent and attach it to the SceneRoot
	DroneAudio = CreateDefaultSubobject<UAkComponent>(TEXT("DroneAudio"));
	DroneAudio->SetupAttachment(DroneMesh);

}

// Called when the game starts or when spawned
void ADroneBase::BeginPlay()
{
	Super::BeginPlay();
	StartingPosition = GetActorLocation();

	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bDoCollisionTest = false;

	//Add Input Mapping Context
	PlayerController = Cast<APlayerController>(Controller);

	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Wwise
	// Start drone audio loop
	DroneAudio->PostAkEvent(WEStart);

	// Set the camera look at zone
	// Delay the overlap check by a short duration
	// This is necessary because the player hasnt spawned yet and is only guaranteed spawned next frame
	GetWorldTimerManager().SetTimerForNextTick([this]()
		{
			TArray<AActor*> OverlappingActors;
	CollisionMesh->GetOverlappingActors(OverlappingActors);

	// Loop through all overlapping actors
	for (AActor* OverlappedActor : OverlappingActors)
	{
		ACameraLookAtZone* lookAtZone{ Cast<ACameraLookAtZone>(OverlappedActor) };

		if (lookAtZone)
		{
			lookAtZone->OnBeginOverlap(nullptr, this, nullptr, 0, false, FHitResult{});
		}
	}
		});
}

// Called every frame
void ADroneBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DoCameraLookAtLogic();

	UpdateDroneSpeedRPTC();

	if (IsOutOfRange())
	{
		ReturnToInitialSpot();
	}

}

// Called to bind functionality to input
void ADroneBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {


		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADroneBase::Move);

		//Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ADroneBase::Interact);
	}
}

void ADroneBase::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = CameraBoom->GetTargetRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);

		// Calculate the tilt 
		float TiltAngleX = FMath::Clamp(MovementVector.X, -1.0f, 1.0f) * TiltX;
		float TiltAngleY = FMath::Clamp(-MovementVector.Y, -1.0f, 1.0f) * TiltY;

		// Set the new rotation with tilt
		FRotator NewRotation = GetActorRotation();
		NewRotation.Roll = FMath::FInterpTo(NewRotation.Roll, TiltAngleX, GetWorld()->GetDeltaSeconds(), 1.0);
		NewRotation.Pitch = FMath::FInterpTo(NewRotation.Pitch, TiltAngleY, GetWorld()->GetDeltaSeconds(), 1.0);
		SetActorRotation(NewRotation);
	}
}

void ADroneBase::Interact(const FInputActionValue& Value)
{
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Interact!"));
	//	DroneMachine->ChangePossession(Cast<APlayerController>(Controller));
}

void ADroneBase::UpdateDroneSpeedRPTC()
{
	const float value{ static_cast<float>(GetVelocity().Length()) / FloatingPawnMovement->GetMaxSpeed() * 100.f };
	DroneAudio->SetRTPCValue(RtpcDroneSpeed, value, 0, RtpcDroneSpeed->GetName());
}

void ADroneBase::ResetDroneSpeedRPTC()
{
	DroneAudio->SetRTPCValue(RtpcDroneSpeed, 0, 0, RtpcDroneSpeed->GetName());
}

void ADroneBase::DoCameraLookAtLogic()
{
	if (CameraLookAtPoint)
	{
		FVector Direction = CameraLookAtPoint->GetComponentLocation() - CameraBoom->GetComponentLocation();
		Direction.Normalize();

		FRotator TargetRotation = Direction.Rotation();
		TargetRotation.Pitch += BoomArmPitchOffset;
		TargetRotation.Roll = 0.f;

		if (!bCameraLookAtInFront)
			TargetRotation.Yaw += 180;


		// Lerp the camera boom's rotation
		float time = GetWorld()->GetDeltaSeconds();
		FRotator NewBoomArmRotation = FMath::RInterpTo(CurrBoomArmRotation, TargetRotation, time, CameraTransitionSpeed * 0.8f);
		FRotator NewCameraRotation = FMath::RInterpTo(FollowCamera->GetRelativeRotation(), FRotator{ CameraPitchOffset ,0,0 }, time, CameraTransitionSpeed * 0.8f);
		float NewTargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, BoomArmLength, time, CameraTransitionSpeed * 0.8f);

		CameraBoom->SetWorldRotation(NewBoomArmRotation);
		FollowCamera->SetRelativeRotation(NewCameraRotation);
		CameraBoom->TargetArmLength = NewTargetArmLength;

		CurrBoomArmRotation = NewBoomArmRotation;
	}
}

void ADroneBase::SetCameraLookAtPoint(USceneComponent* lookAtPoint, bool inFront, float boomArmPitch, float cameraPitch, float boomArmLength, float transitionSpeed)
{
	CameraLookAtPoint = lookAtPoint;
	bCameraLookAtInFront = inFront;
	BoomArmPitchOffset = boomArmPitch;
	CameraPitchOffset = cameraPitch;
	BoomArmLength = boomArmLength;
	CameraTransitionSpeed = transitionSpeed;
}

bool ADroneBase::IsOutOfRange()
{

	float distance = FVector::Distance(StartingPosition, GetActorLocation());
	if (distance >= MaxRange)
	{
		return true;
	}
	return false;
}

void ADroneBase::ReturnToInitialSpot()
{
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	UKismetSystemLibrary::MoveComponentTo(DroneRoot, StartingPosition, StartingRotation, false, false, 1.f, false, EMoveComponentAction::Type::Move, LatentInfo);
	ResetDroneSpeedRPTC();
}
