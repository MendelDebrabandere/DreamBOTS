// Fill out your copyright notice in the Description page of Project Settings.


#include "GrabbingMachine.h"
#include "DreamBotsCharacter.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkAudioEvent.h"

// Sets default values
AGrabbingMachine::AGrabbingMachine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    SceneComponent->SetupAttachment(RootComponent);

	// Create a ToyBall Mesh
    SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ClawMesh"));
    SkeletalMeshComponent->SetupAttachment(SceneComponent);

	// Create a Box Collision Component
	BoxCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollisionComponent->SetupAttachment(SkeletalMeshComponent);
    BoxCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AGrabbingMachine::OnOverlapBegin);

    WayPointUp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WayPointUp"));
    WayPointUp->SetupAttachment(RootComponent);

    WayPointSideway = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WayPointSide"));
    WayPointSideway->SetupAttachment(RootComponent);

    ArmExtender = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArmExtender"));
    ArmExtender->SetupAttachment(SkeletalMeshComponent);

    Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("DangerDecal"));
    Decal->SetupAttachment(SceneComponent);

}

// Called when the game starts or when spawned
void AGrabbingMachine::BeginPlay()
{
	Super::BeginPlay();
	
    InitialLocation = GetActorLocation();
    bInitHorX = bHorizontalInX;
    bInitHorY = bHorizontalInY;
    bInitVert = bVertical;

    if(bInitVert)
    {
        Decal->SetVisibility(false);
    }
}

void AGrabbingMachine::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Check if the Overlapping Actor is the Player Character
    ADreamBotsCharacter* PlayerCharacter = Cast<ADreamBotsCharacter>(OtherActor);

    if(PlayerCharacter && !bGrabbed)
    {
        BoxCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		bGrabbed = true;
        bClose = true;

        bHorizontalInX = false;
        bHorizontalInY = false;
        bVertical = false;

        SetActorLocation(FVector(PlayerCharacter->GetActorLocation().X, PlayerCharacter->GetActorLocation().Y, PlayerCharacter->GetActorLocation().Z - 100.0));

        PlayerCharacter->DisableInput(nullptr);
        PlayerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
        PlayerCharacter->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false));

        // Wwise 
        // Grabbing audio
        FOnAkPostEventCallback nullCallback{};
        WEGrab->PostOnActor(this, nullCallback, 0, false);

        MoveMachineUp();

        ReleasePlayerDelegate.BindUFunction(this, FName("ReleasePlayer"), PlayerCharacter);
        GetWorldTimerManager().SetTimer(MovementTimer, ReleasePlayerDelegate, 2.f, false);
        GetWorldTimerManager().SetTimer(MovementTimerSide, this, &AGrabbingMachine::MoveMachineSideway, 1.f, false);
        GetWorldTimerManager().SetTimer(GrabTimer, this, &AGrabbingMachine::DisableGrab, 2.f, false);
        GetWorldTimerManager().SetTimer(MoveInitTimer, this, &AGrabbingMachine::MoveToInitalPos, 3.f, false);
        GetWorldTimerManager().SetTimer(IdleTimer, this, &AGrabbingMachine::Idle, 4.f, false);
    }
}

// Called every frame
void AGrabbingMachine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (bHorizontalInX)
    {
        float CurrentTime = GetGameTimeSinceCreation() - bTime;
        float OffsetX = Amplitude * FMath::Sin(CurrentTime * Speed);

        FVector NewLocation = InitialLocation + FVector(OffsetX, 0.0f, 0.0f);

        SetActorLocation(NewLocation);
    }
    else if(bHorizontalInY)
    {
        float CurrentTime = GetGameTimeSinceCreation();
        float OffsetY = Amplitude * FMath::Sin(CurrentTime * Speed);

        FVector NewLocation = InitialLocation + FVector(0.0f, OffsetY, 0.0f);

        SetActorLocation(NewLocation);
    }
    else if (bVertical)
    {
        float CurrentTime = GetGameTimeSinceCreation();
        float OffsetZ = Amplitude * FMath::Sin(CurrentTime * Speed);

        FVector NewLocation = InitialLocation + FVector(0.0f, 0.0f, OffsetZ);

        if (NewLocation.Z < WorldZGrab)
        {
            bClose = true;
        }
        else bClose = false;

        SetActorLocation(NewLocation);
    }
}

void AGrabbingMachine::MoveMachineUp()
{
    // Get the Root Component of the Machine
    USceneComponent* RootComponentMach = GetRootComponent();

    FVector ResetLocationUp = WayPointUp->GetComponentLocation() - GetActorLocation();

    // Save the Location the Machine is supposed to move to
    FVector MachineTargetLocation = RootComponentMach->GetComponentLocation() + ResetLocationUp;

    EMoveComponentAction::Type MoveActionFlag = EMoveComponentAction::Move;
    FLatentActionInfo LatenInfo;
    LatenInfo.CallbackTarget = this;

    // Move Player to Desired Location in 2 Seconds
    // Since this is hard coded Wwise is also hardcoded if 2 seconds changes Wwise should also change <-- IMPORTANT
    UKismetSystemLibrary::MoveComponentTo(RootComponentMach, MachineTargetLocation, GetActorRotation(), false, false, 1.f, false, MoveActionFlag, LatenInfo);
}

void AGrabbingMachine::MoveMachineSideway()
{
    USceneComponent* RootComponentMach = GetRootComponent();

    FVector ResetLocationSideway = WayPointSideway->GetComponentLocation() - WayPointUp->GetComponentLocation();

    FVector MachineTargetLocation = RootComponentMach->GetComponentLocation() + ResetLocationSideway;

    EMoveComponentAction::Type MoveActionFlag = EMoveComponentAction::Move;
    FLatentActionInfo LatenInfo;
    LatenInfo.CallbackTarget = this;

    UKismetSystemLibrary::MoveComponentTo(RootComponentMach, MachineTargetLocation, GetActorRotation(), false, false, 1.f, false, MoveActionFlag, LatenInfo);
}

void AGrabbingMachine::ReleasePlayer(ADreamBotsCharacter* PlayerCharacter)
{
    PlayerCharacter->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false));
    PlayerCharacter->EnableInput(nullptr);
    PlayerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

    // Wwise 
    // Releasing audio
    FOnAkPostEventCallback nullCallback{};
    WERelease->PostOnActor(this, nullCallback, 0, false);
    PlayerCharacter->FallingAudio();
}

void AGrabbingMachine::DisableGrab()
{
    bGrabbed = false;
    bClose = false;
}

void AGrabbingMachine::MoveToInitalPos()
{
    USceneComponent* RootComponentMach = GetRootComponent();

    EMoveComponentAction::Type MoveActionFlag = EMoveComponentAction::Move;
    FLatentActionInfo LatenInfo;
    LatenInfo.CallbackTarget = this;

    UKismetSystemLibrary::MoveComponentTo(RootComponentMach, InitialLocation, GetActorRotation(), false, false, 1.f, false, MoveActionFlag, LatenInfo);
}

void AGrabbingMachine::Idle()
{
    BoxCollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    if(bInitHorX)
    {
        bHorizontalInX = true;
    }

    if(bInitHorY)
    {
        bHorizontalInY = true;
    }

    if(bInitVert)
    {
        bVertical = true;
    }

    bTime = GetGameTimeSinceCreation();
}


