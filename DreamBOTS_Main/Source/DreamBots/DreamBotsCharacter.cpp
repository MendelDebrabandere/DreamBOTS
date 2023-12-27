// Copyright Epic Games, Inc. All Rights Reserved.

#include "DreamBotsCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DroneMachine.h"
#include "Trampolin.h"
#include "BunnyMachine.h"
#include "CameraLookAtZone.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SaveGameState.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkComponent.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkSwitchValue.h"
#include "NiagaraComponent.h"


//////////////////////////////////////////////////////////////////////////
// ADreamBotsCharacter

ADreamBotsCharacter::ADreamBotsCharacter()
{
	//Set this actor to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 1.f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->MaxAcceleration = 3000.f;
	GetCharacterMovement()->RotationRate = FRotator{ 0.f,700.f,0.f };

	// Create a DroneMesh
	DroneMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DroneMesh"));
	DroneMeshComponent->SetupAttachment(RootComponent);

	// Load Drone Mesh
	DroneMesh = LoadObject<UStaticMesh>(nullptr, *DroneMeshPath);
	DroneMeshComponent->SetStaticMesh(DroneMesh);

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraRotationLagSpeed = 3.f;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// CharacterSwapSmoke
	CharacterSwapSmokeComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("CharacterSwapSmokeComponent"));
	CharacterSwapSmokeComponent->SetupAttachment(RootComponent);
	CharacterSwapSmokeComponent->bAutoActivate = false;

	WalkTrailComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WalkTrail"));
	WalkTrailComponent->SetupAttachment(RootComponent);

	// Addons
	AddonMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AddonMeshComp"));
	AddonMeshComp->SetupAttachment(GetMesh());
	AddonMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CleaningBroomMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CleaningBroomMesh"));
	CleaningBroomMesh->SetupAttachment(AddonMeshComp);
	CleaningBroomMesh->SetVisibility(false);
	CleaningBroomMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Overlap
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ADreamBotsCharacter::OnOverlapBegin);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ADreamBotsCharacter::OnOverlapEnd);

	// Create AkComponent for the pickup audio
	PickupAudio = CreateDefaultSubobject<UAkComponent>(TEXT("PickupAudio"));
	PickupAudio->SetupAttachment(RootComponent);

	// Create AkComponent for the movement audio
	MovementAudio = CreateDefaultSubobject<UAkComponent>(TEXT("MovementAudio"));
	MovementAudio->SetupAttachment(RootComponent);

	CharMesh = GetMesh();

	ActorsToCheck.Add(ABunnyMachine::StaticClass());

	MovieTapesFound.Init(false, 16);

	CleaningNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(FName("CleaningNiagaraComponent"));
	CleaningNiagaraComponent->SetupAttachment(CharMesh);

}

void ADreamBotsCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	DroneMeshComponent->SetHiddenInGame(true, true);

	ZvalueThreshold = GetActorLocation().Z - 1500.0;

	if (CleaningNiagaraSystem)
	{
		CleaningNiagaraComponent->SetAsset(CleaningNiagaraSystem);
		//CleaningNiagaraComponent->Deactivate();
	}


	CharacterSwapSmokeComponent->bAutoActivate = false;
	LoadGame();
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		DreamBotsController = PlayerController;
	}

	//Set values
	bThresholdReached = false;
	bOnGround = false;
	LastSavedLocation = GetActorLocation();
	//DroneMeshComponent->SetHiddenInGame(true, true);

	//Start the timer to periodically call the SaveLocation function
	GetWorldTimerManager().SetTimer(SaveLocationTimer, this, &ADreamBotsCharacter::SaveLocation, SaveInterval, true);

	//Start as arms robot
	//ChangeCharacterType(Arms, false, true);

	// Set the camera look at zone
	// Delay the overlap check by a short duration
	// This is necessary because the player hasnt spawned yet and is only guaranteed spawned next frame
	GetWorldTimerManager().SetTimerForNextTick([this]()
		{
			CheckForCameraZone();
			InstantSnapCamera();
		});

}

void ADreamBotsCharacter::Tick(float DeltaSeconds)
{
	//Call the base class
	Super::Tick(DeltaSeconds);

	if(GetCharacterMovement()->Velocity.SizeSquared() < 0.5f)
	{
		WalkTrailComponent->Deactivate();
	}

	if (bDisableTick)
		return;

	if(bThresholdReached)
	{
		if(!bResetStepOne)
		{
			SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, FMath::Lerp(GetActorLocation().Z, Distance, DeltaSeconds * 1.7f)));

			bIsPickedUp = true;

			if (FVector::DistSquared(GetActorLocation(), DistanceLocation) < FMath::Square(70.f))
			{
				bResetStepOne = true;
				PickupAudio->PostAkEvent(WETopReached);
			}
		}

		if(bResetStepOne)
		{
			SetActorLocation((FMath::Lerp(GetActorLocation(), FVector(LastSavedLocation.X, LastSavedLocation.Y, GetActorLocation().Z), DeltaSeconds * 2.2f)));

			if (FVector::DistSquared(GetActorLocation(), FVector(LastSavedLocation.X, LastSavedLocation.Y, GetActorLocation().Z)) < FMath::Square(20.f))
			{
				bThresholdReached = false;
				bResetStepOne = false;
				bIsPickedUp = false;
				bReset = false;

				FallingAudio();

				EnableInput(nullptr);
				GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
			}
		}
	}

	if(GetActorLocation().Z < ZvalueThreshold + 700.0)
	{
		if (bDroneStartedChase)
		{
			bDroneStartedChase = false;

			if (WEDroneDown)
			{
				PickupAudio->PostAkEvent(WEDroneDown);
			}
		}

		DroneMeshComponent->SetHiddenInGame(false, true);
		DroneMeshComponent->SetWorldLocation(FMath::Lerp(DroneMeshComponent->GetComponentLocation(), GetActorLocation() + FVector(0.0, 0.0, 175.0), DeltaSeconds * 8.f));
	}
	else if(!bDroneStartedChase)
	{
		bDroneStartedChase = true;
	}

	if(!bIsPickedUp && !bReset)
	{
		if (FVector::DistSquared(DroneMeshComponent->GetComponentLocation(), GetActorLocation() + FVector(0.0, 0.0, 1500.0)) < FMath::Square(1.f))
		{
			DroneMeshComponent->SetHiddenInGame(true, true);
			bReset = true;
		}

		DroneMeshComponent->SetWorldLocation(FMath::Lerp(DroneMeshComponent->GetComponentLocation(), GetActorLocation() + FVector(0.0, 0.0, 1500.0), DeltaSeconds * 2.f));
	}
	if (!bIsRiding)
	{
		bIsAllowedToJump = true;
	}
	if (HasAnimationOfPickUpEnded)
	{
		AllowPlayerToMove = true;
		bIsAllowedToJump = true;
		HasPickuedUpATape = false;
		HasAnimationOfPickUpEnded = false;
	}

	StartRidingCooldown(DeltaSeconds);
	DoPlayerFalloffLogic();
	DoCameraLookAtLogic();
	RotateCleaningBroom(DeltaSeconds);
	UpdatePickupSpeedRPTC();
	CheckIfLandedAfterPickup();
	BroomTouchingGround();

	bWasOnGround = bOnGround;
}

void ADreamBotsCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	SaveGame();
	//Clear the timer when the actor is destroyed
	GetWorldTimerManager().ClearTimer(SaveLocationTimer);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ADreamBotsCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ADreamBotsCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADreamBotsCharacter::Move);

		//Interact
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ADreamBotsCharacter::Interact);

		//Dismount
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ADreamBotsCharacter::Dismounting);
	}

}

void ADreamBotsCharacter::Interact(const FInputActionValue& Value)
{

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Overlapped!"));
	if (PlayerInRangeOfDroneMachine && DroneMachine && DroneMachine->GetDroneMachineActivity())
	{
		IsEnteringTheDrone = true;
		GetCharacterMovement()->Velocity = {};
		DroneMachine->SetPlayer(this);
		DroneMachine->ChangePossession(Cast<APlayerController>(Controller));
	}
	if (MazeConsole && MazeConsole->GetMazeConsoleActivity())
	{
		//	 GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Activated!"));
		bIsAllowedToJump = false;
		MazeConsole->ActivateToyBalls();
		MazeConsole->DisableMachine();
	}
	else if (MazeConsole && !MazeConsole->GetMazeConsoleActivity())
	{
		bIsAllowedToJump = true;
	}


	if (HasPickuedUpATape)
	{
		OnOverlappedWithTape.Broadcast(false);
	}
	if (MountableActor && bIsCloseToActor && (!bIsRiding && RidingCooldown <= 0.f) && !GetCharacterMovement()->IsFalling())
	{

		SetActorEnableCollision(false);
		AttachToComponent(MountableActor->RideRoot, FAttachmentTransformRules::SnapToTargetIncludingScale);
		MountableActor->Mounted();

		SetCanPlayerMove(false);
		bIsRiding = true;
		RidingCooldown = .5f;
	}
}
void ADreamBotsCharacter::Dismounting()
{
	if (bIsRiding)
	{
		SetActorEnableCollision(true);
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		SetCanPlayerMove(true);
		MountableActor->Dismount();
		
		HasRidingCooldownStarted = true;
		bIsRiding = false;
		bIsAllowedToJump = true;
	}
}

void ADreamBotsCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Overlapped!"));
	
	// Try to use if else instead of multiple if-statements
	if (ADroneMachine* Dronemachine = Cast<ADroneMachine>(OtherActor))
	{
		DroneMachine = Dronemachine;
		PlayerInRangeOfDroneMachine = true;
	}
	else if (AMazeConsole* Mazeconsole = Cast<AMazeConsole>(OtherActor))
	{
			MazeConsole = Mazeconsole;
	}
	else if (AMountableActor* Mountableactor = Cast<AMountableActor>(OtherActor) )
	{
		MountableActor = Mountableactor;
		bIsCloseToActor = true;
		bIsAllowedToJump = false;
	}
}

void ADreamBotsCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Try to use if else instead of multiple if-statements
	if (ADroneMachine* Dronemachine = Cast<ADroneMachine>(OtherActor))
	{
		PlayerInRangeOfDroneMachine = false;
	}
	else if (AMountableActor* Mountableactor = Cast<AMountableActor>(OtherActor))
	{
		//MountableActor = nullptr;
		bIsCloseToActor = false;
	}
}

void ADreamBotsCharacter::ChangeCharacterType(CharacterType type, bool activateEffect, bool rememberType)
{
	//const FString TypeName = UEnum::GetValueAsString(type);

	//dont do anything if it is already the specified type
	if (CharType == type)
		return;

	if (rememberType)
		OldCharType = type;

	// If no nullptrs
	if (CharacterSwapSmokeComponent)
	{
		//if the character is already transforming
		if (CharacterSwapSmokeComponent->IsActive() && activateEffect)
		{
			CharacterSwapSmokeComponent->Deactivate();
			CharacterSwapSmokeComponent->ResetSystem();
		}

		//turn on smoke effect
		if (activateEffect)
			CharacterSwapSmokeComponent->ActivateSystem();

		if (CharType == Cleaning)
		{
			MovementAudio->PostAkEvent(WEStopBroom);
		}

		CharType = type;

		//assing the mesh in 0.3 sec
		//FTimerHandle SwapTimerHandle;
		//GetWorld()->GetTimerManager().SetTimer(SwapTimerHandle, this, &ADreamBotsCharacter::SwapAddonMesh, 0.3f, false);
		SwapAddonMesh(activateEffect);
	}
}

void ADreamBotsCharacter::GoBackToOldType()
{
	ChangeCharacterType(OldCharType, true, false);
}

void ADreamBotsCharacter::ChangeCharacterColor(CharacterSkinColor color, bool activateEffect)
{
	//dont do anything if it is already the specified color
	if (CharSkinColor == color)
		return;


	// If no nullptrs
	if (CharacterSwapSmokeComponent)
	{
		//if the character is already transforming
		if (CharacterSwapSmokeComponent->IsActive() && activateEffect)
		{
			CharacterSwapSmokeComponent->Deactivate();
			CharacterSwapSmokeComponent->ResetSystem();
		}

		//turn on smoke effect
		if (activateEffect)
			CharacterSwapSmokeComponent->ActivateSystem();

		CharSkinColor = color;

		// Wwise
		// Color swap audio
		if (activateEffect)
			MovementAudio->PostAkEvent(WEColorSwap);

		//assing the color in 0.5 sec
		//FTimerHandle SwapTimerHandle;
		//GetWorld()->GetTimerManager().SetTimer(SwapTimerHandle, this, &ADreamBotsCharacter::SwapColorSkin, 0.5f, false);
		SwapColorSkin();
	}
}

void ADreamBotsCharacter::StartCleaning()
{

	MovementAudio->PostAkEvent(WEStartCleaning);
	CleaningNiagaraComponent->Activate();
}

void ADreamBotsCharacter::RotateCleaningBroom(float elapsed)
{
	if (EnableCleaning)
	{
		//Rotation
		CleaningBroomMesh->AddLocalRotation(RotationDirection * elapsed * RotationCleaningSpeed);
	}
	else
	{
		CleaningBroomMesh->AddLocalRotation({0.f,0.f,0.f});
	}
}

void ADreamBotsCharacter::StopCleaning()
{
	MovementAudio->PostAkEvent(WEStopCleaning);
	CleaningNiagaraComponent->Deactivate();
}

void ADreamBotsCharacter::FallingAudio()
{
	// Wwise post release event
	PickupAudio->PostAkEvent(WERelease);
	bIsFallingAfterPickup = true;
}

void ADreamBotsCharacter::CheckForCameraZone()
{
	TArray<AActor*> OverlappingActors;
	GetCapsuleComponent()->GetOverlappingActors(OverlappingActors);

	// Loop through all overlapping actors
	for (AActor* OverlappedActor : OverlappingActors)
	{
		ACameraLookAtZone* lookAtZone{ Cast<ACameraLookAtZone>(OverlappedActor) };

		if (lookAtZone)
		{
			lookAtZone->OnBeginOverlap(nullptr, this, nullptr, 0, false, FHitResult{});
		}
	}
}

void ADreamBotsCharacter::FootStepEvent(bool leftFoot)
{
	constexpr float MaxTraceDistance{ 150 };
	FVector Start = GetActorLocation(); // Starting at the character's location
	FVector End = Start - FVector(0, 0, MaxTraceDistance); // End point of the trace directly below the character

	FHitResult Hit;
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(FootStepEvent), true, this);
	TraceParams.bReturnPhysicalMaterial = true; // Ensure that we want to get the physical material

	// Perform the trace
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		OUT Hit,
		Start,
		End,
		ECC_Visibility,
		TraceParams
	);

	if (bHit)
	{
		if (Hit.PhysMaterial.IsValid())
		{
			UPhysicalMaterial* PhysMat = Hit.PhysMaterial.Get();

			FString SurfaceString{ "Surface" };

			if (PhysMat->SurfaceType == SurfaceType1) //Grass
			{
				MovementAudio->SetSwitch(SwitchGrass, SurfaceString, "Grass");
			}
			else if (PhysMat->SurfaceType == SurfaceType2) //Metal
			{
				MovementAudio->SetSwitch(SwitchMetal, SurfaceString, "Metal");
			}
			else if (PhysMat->SurfaceType == SurfaceType3) //Rock
			{
				MovementAudio->SetSwitch(SwitchRock, SurfaceString, "Rock");
			}
			else if (PhysMat->SurfaceType == SurfaceType4) //Plastic
			{
				MovementAudio->SetSwitch(SwitchPlastic, SurfaceString, "Plastic");
			}

			MovementAudio->PostAkEvent(WEFootstep);
		}
	}
}

void ADreamBotsCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr && AllowPlayerToMove)
	{
		// find out which way is forward based on camera direction
		const FRotator Rotation = CameraBoom->GetTargetRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ADreamBotsCharacter::DoPlayerFalloffLogic()
{
	//Save Location of Actor every frame
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0.0, 0.0, 100.0);
	FHitResult HitResult;

	//Ignore the player so it doesnt collide with the line trace
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	if(TrampolineActor)
	CollisionParams.AddIgnoredActor(TrampolineActor);

	//Shoot a Line Trace down -> In case it hits a platform Player is on Ground
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
	{
		for (const TSubclassOf<AActor>& ActorClass : ActorsToCheck)
		{
			if (HitResult.GetActor() && HitResult.GetActor()->IsA(ActorClass))
			{
				bShouldSave = false;
				break;
			}
			else
			{
				bShouldSave = true;
			}
		}

		//bOnGround = true;
		bOnGroundSound = true;
		bTouchedTrampoline = false;
		WalkTrailComponent->Activate();
	}
	else
	{
		//bOnGround = false;
		bOnGroundSound = false;
		WalkTrailComponent->Deactivate();
	}

	bOnGround = GetMovementComponent()->IsMovingOnGround();

	if(bTouchedTrampoline)
	{
		DisableInput(nullptr);
	}
	else
	{
		EnableInput(nullptr);
	}

	//In case Player falls off a Platform call MoveUpwards() function -> This moves the player back to the center of the platform
	if (GetActorLocation().Z < ZvalueThreshold && !bThresholdReached)
	{
		Distance = ((LastSavedLocation.Z - ZvalueThreshold) + ZvalueOffset) + GetActorLocation().Z;
		DistanceLocation = FVector(GetActorLocation().X, GetActorLocation().Y, Distance);
		DisableInput(nullptr);
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		//MoveUpwards(Distance);
		PickupAudio->PostAkEvent(WEPickup);
		bThresholdReached = true;
	}
}

void ADreamBotsCharacter::SaveLocation()
{
	//Save new Location of Player ONLY if standing on a Platform
	if (bOnGround && bShouldSave)
	{
		LastSavedLocation = GetActorLocation();
	}
}

void ADreamBotsCharacter::HideDrone()
{
	//DroneMeshComponent->SetHiddenInGame(true, true);

	FallingAudio();

	// Reset picked up
	bIsPickedUp = false;
	bReset = false;

	// Reset player velocity
	GetMovementComponent()->Velocity = FVector{ 0.f,0.f,0.f };
}

void ADreamBotsCharacter::EnableMovementInput()
{
	DreamBotsController->SetIgnoreMoveInput(false);
}

void ADreamBotsCharacter::DoCameraLookAtLogic()
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

void ADreamBotsCharacter::BroomTouchingGround()
{
	if (CharType == Cleaning)
	{
		if (bWasOnGround && !bOnGround)
		{
			MovementAudio->PostAkEvent(WEStopBroom);
		}
		else if (!bWasOnGround && bOnGround)
		{
			MovementAudio->PostAkEvent(WEStartBroom);
		}
	}
}

void ADreamBotsCharacter::StartRidingCooldown(float DeltaSeconds)
{
	if (HasRidingCooldownStarted)
	{
		if (RidingCooldown <= 0.f)
		{
			HasRidingCooldownStarted = false;
		}
		else
		{
			RidingCooldown -= DeltaSeconds;
		}
	}
}



void ADreamBotsCharacter::InstantSnapCamera()
{
	if (!CameraLookAtPoint)
		return;

	FVector Direction = CameraLookAtPoint->GetComponentLocation() - CameraBoom->GetComponentLocation();
	Direction.Normalize();

	FRotator TargetRotation = Direction.Rotation();
	TargetRotation.Pitch += BoomArmPitchOffset;
	TargetRotation.Roll = 0.f;

	if (!bCameraLookAtInFront)
		TargetRotation.Yaw += 180;


	CameraBoom->SetWorldRotation(TargetRotation);
	FollowCamera->SetRelativeRotation(FRotator{ CameraPitchOffset ,0,0 });
	CameraBoom->TargetArmLength = BoomArmLength;

	CurrBoomArmRotation = TargetRotation;

}

void ADreamBotsCharacter::SwapAddonMesh(bool activateEffect)
{
	AddonMeshComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	CleaningBroomMesh->SetVisibility(false, true);
	EnableCleaning = false;
	FVector newLocation = CleaningBroomMesh->GetRelativeLocation();
	switch (CharType)
	{
	case Cleaning:
		EnableCleaning = true;
		AddonMeshComp->SetStaticMesh(CleaningAddon);
		AddonMeshComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("BellyAddonSocket"));
		
		CleaningBroomMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		CleaningBroomMesh->SetVisibility(true, true);
		CleaningBroomMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		CleaningBroomMesh->AttachToComponent(AddonMeshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("BroomSocket"));
		CleaningBroomMesh->SetRelativeLocation(FVector(newLocation.X, newLocation.Y, 9.0f));

		if (activateEffect)
		{
			MovementAudio->PostAkEvent(WECleaning);
			MovementAudio->PostAkEvent(WEStartBroom);
		}
		break;
	case Cooking:
		AddonMeshComp->SetStaticMesh(CookingAddon);
		AddonMeshComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("HatAddonSocket"));

		if (activateEffect)
			MovementAudio->PostAkEvent(WECooking);
		break;
	case Arms:
		AddonMeshComp->SetStaticMesh(ArmsAddon);
		AddonMeshComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("BackAddonSocket"));

		if (activateEffect)
			MovementAudio->PostAkEvent(WEArms);
		break;
	case Typing:
		AddonMeshComp->SetStaticMesh(TypingAddon);
		AddonMeshComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("BellyAddonSocket"));

		if (activateEffect)
			MovementAudio->PostAkEvent(WEType);
		break;
	case Basic:
		AddonMeshComp->SetVisibility(false);

		if (activateEffect)
			MovementAudio->PostAkEvent(WENothing);
	}
}

void ADreamBotsCharacter::SwapColorSkin()
{
	switch (CharSkinColor)
	{
	case Yellow:
		CharMesh->SetMaterial(0, MatYellow);
		break;
	case Red:
		CharMesh->SetMaterial(0, MatRed);
		break;
	case Blue:
		CharMesh->SetMaterial(0, MatBlue);
		break;
	case Purple:
		CharMesh->SetMaterial(0, MatPurple);
		break;
	case Orange:
		CharMesh->SetMaterial(0, MatOrange);
		break;
	case Black:
		CharMesh->SetMaterial(0, MatBlack);
		break;
	case Cyan:
		CharMesh->SetMaterial(0, MatCyan);
		break;
	case White:
		CharMesh->SetMaterial(0, MatWhite);
	}
}

void ADreamBotsCharacter::SaveGame()
{

	UDreamBotsGameInstance* instance = Cast<UDreamBotsGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	USaveGameState* saveState = Cast<USaveGameState>(UGameplayStatics::CreateSaveGameObject(USaveGameState::StaticClass()));
	if (instance)
	{
		instance->MovieTapesArray = MovieTapesFound;
	}
	if (saveState)
	{
		saveState->FoundMovieTapes = MovieTapesFound;
		UGameplayStatics::SaveGameToSlot(saveState, "Slot", 0);
	}
}

void ADreamBotsCharacter::LoadGame()
{
	USaveGameState* saveState = Cast<USaveGameState>(UGameplayStatics::GetGameInstance(GetWorld()));
	saveState = Cast<USaveGameState>(UGameplayStatics::LoadGameFromSlot("Slot", 0));
	if (saveState)
	{
		this->MovieTapesFound = saveState->FoundMovieTapes;
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Loaded!"));

	}
}

void ADreamBotsCharacter::SetCameraLookAtPoint(USceneComponent* lookAtPoint, bool inFront, float boomArmPitch, float cameraPitch, float boomArmLength, float transitionSpeed)
{
	CameraLookAtPoint = lookAtPoint;
	bCameraLookAtInFront = inFront;
	BoomArmPitchOffset = boomArmPitch;
	CameraPitchOffset = cameraPitch;
	BoomArmLength = boomArmLength;
	CameraTransitionSpeed = transitionSpeed;
}

void ADreamBotsCharacter::AddMovieTape(DrawingType idx)
{
	++CurrentAmountOfMovieTapes;

	MovieTapesFound[(int)idx] = true;
	HasPickuedUpATape = true;
	// Wwise
	// Tape pickup
	PickupAudio->PostAkEvent(WETapePickup);


	UDreamBotsGameInstance* instance = Cast<UDreamBotsGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (instance)
	{
		instance->CurrentMovieTape = idx;
	}
	OnOverlappedWithTape.Broadcast(true);
	AllowPlayerToMove = false;
	bIsAllowedToJump = false;
	SaveGame();
}

void ADreamBotsCharacter::UpdatePickupSpeedRPTC()
{
	if (!bIsPickedUp)
		return;

	const float value{ static_cast<float>(GetVelocity().Length()) / RtpcMaxVelocity * 100.f };
	PickupAudio->SetRTPCValue(RtpcPickupSpeed, value, 0, RtpcPickupSpeed->GetName());
}

void ADreamBotsCharacter::CheckIfLandedAfterPickup()
{
	if (bIsFallingAfterPickup && bOnGroundSound)
	{
		bIsFallingAfterPickup = false;

		// Wwise
		// Grounded event
		PickupAudio->PostAkEvent(WEGrounded);
	}
}

bool ADreamBotsCharacter::CanJump()
{
	return bIsAllowedToJump;
}

void ADreamBotsCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	constexpr float MaxTraceDistance{ 150 };
	FVector Start = GetActorLocation(); // Starting at the character's location
	FVector End = Start - FVector(0, 0, MaxTraceDistance); // End point of the trace directly below the character

	FHitResult HitR;
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(FootStepEvent), true, this);
	TraceParams.bReturnPhysicalMaterial = true; // Ensure that we want to get the physical material

	// Perform the trace
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		OUT HitR,
		Start,
		End,
		ECC_Visibility,
		TraceParams
	);

	if (bHit)
	{
		if (HitR.PhysMaterial.IsValid())
		{
			UPhysicalMaterial* PhysMat = HitR.PhysMaterial.Get();


			FString SurfaceString{ "Surface" };

			if (PhysMat->SurfaceType == SurfaceType1) //Grass
			{
				MovementAudio->SetSwitch(SwitchGrass, SurfaceString, "Grass");
			}
			else if (PhysMat->SurfaceType == SurfaceType2) //Metal
			{
				MovementAudio->SetSwitch(SwitchMetal, SurfaceString, "Metal");
			}
			else if (PhysMat->SurfaceType == SurfaceType3) //Rock
			{
				MovementAudio->SetSwitch(SwitchRock, SurfaceString, "Rock");
			}
			else if (PhysMat->SurfaceType == SurfaceType4) //Plastic
			{
				MovementAudio->SetSwitch(SwitchPlastic, SurfaceString, "Plastic");
			}

			MovementAudio->PostAkEvent(WELand);
		}
	}
}

void ADreamBotsCharacter::Jump()
{
	if (CanJump() && AllowPlayerToMove)
	{
		if (bOnGround)
		{
			MovementAudio->PostAkEvent(WEJump);
		}

		ACharacter::Jump();
	}
}

void ADreamBotsCharacter::SetCameraCollision(bool value)
{
	CameraBoom->bDoCollisionTest = value;
}
