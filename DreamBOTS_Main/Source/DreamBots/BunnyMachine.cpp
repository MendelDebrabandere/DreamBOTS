// Fill out your copyright notice in the Description page of Project Settings.


#include "BunnyMachine.h"
#include "DreamBotsCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NiagaraComponent.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkAudioEvent.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkComponent.h"

// Sets default values
ABunnyMachine::ABunnyMachine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Scene Component
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SceneComponent->SetupAttachment(RootComponent);

	// Create a Carrot Mesh
	CarrotMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarrotMesh"));
	CarrotMeshComponent->SetupAttachment(SceneComponent);

	// Create a Carrot Ring Mesh
	CarrotRingMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarrotRing"));
	CarrotRingMeshComponent->SetupAttachment(SceneComponent);

	// Create a Box Collision Component
	BoxCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollisionComponent->SetupAttachment(CarrotMeshComponent);

	BunnyBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BunnyBoxComp"));
	BunnyBoxComp->SetupAttachment(SceneComponent);

	NoShakeCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("NoShakeCollision"));
	NoShakeCollisionComponent->SetupAttachment(SceneComponent);

	// Create AkComponent for the carrot audio
	CarrotAudio = CreateDefaultSubobject<UAkComponent>(TEXT("CarrotAudio"));
	CarrotAudio->SetupAttachment(CarrotMeshComponent);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(FName("NiagaraComponentRight"));
	NiagaraComponent->SetupAttachment(SceneComponent);

}

// Called when the game starts or when spawned
void ABunnyMachine::BeginPlay()
{
	Super::BeginPlay();

	InitialActorLocation = GetActorLocation();

	InitialLocation = CarrotMeshComponent->GetComponentLocation();

	NoShakeCollisionComponent->SetWorldLocation(InitialLocation + FVector(0.0, 0.0, 250.0));

	InitialComponentLocation = NoShakeCollisionComponent->GetComponentLocation();

	BoxCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABunnyMachine::OnOverlapBegin);
	BunnyBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ABunnyMachine::OnOverlapBeginBunny);
	BunnyBoxComp->OnComponentEndOverlap.AddDynamic(this, &ABunnyMachine::OnOverlapEndBunny);
}

// Called every frame
void ABunnyMachine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bHitCarrot)
	{
		FVector Offset = FMath::VRand() * 2.f;
		Offset.X = 0.0;
		Offset.Y = 0.0; 
		CarrotMeshComponent->AddRelativeLocation(Offset);

		if(!MaterialsRing.IsEmpty())
		{
			AccumulatedTime += DeltaTime;

			if(AccumulatedTime < 0.3f)
			{
				CarrotRingMeshComponent->SetMaterial(0, MaterialsRing[1]);
			}
			else if(AccumulatedTime > 0.3f && AccumulatedTime < 0.6f)
			{
				CarrotRingMeshComponent->SetMaterial(0, MaterialsRing[0]);
			}
			else if(AccumulatedTime > 0.6f)
			{
				AccumulatedTime = 0.f;
			}
		}
	}

	if(bMoveDown)
	{
		NoShakeCollisionComponent->SetWorldLocation(CarrotMeshComponent->GetComponentLocation() + FVector(0.0f, 0.0f, 250.0));
	}

	if(bMoveUp)
	{
		NoShakeCollisionComponent->SetWorldLocation((FMath::Lerp(NoShakeCollisionComponent->GetComponentLocation(), InitialLocation + FVector(0.0, 0.0,500.0), DeltaTime * 2.0)));
		CarrotMeshComponent->SetWorldLocation((FMath::Lerp(CarrotMeshComponent->GetComponentLocation(), InitialLocation + FVector(0.0, 0.0, 250.0), DeltaTime * 2.0)));
	}

	if(bReset)
	{
		CarrotMeshComponent->SetWorldLocation((FMath::Lerp(CarrotMeshComponent->GetComponentLocation(), InitialLocation, DeltaTime * 5.0)));

		if(FVector::DistSquared(CarrotMeshComponent->GetComponentLocation(), InitialLocation) < FMath::Square(1.f))
		{
			bReset = false;
		}
	}

}

void ABunnyMachine::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the Overlapping Actor is the Player Character
	ADreamBotsCharacter* PlayerCharacter = Cast<ADreamBotsCharacter>(OtherActor);

	if(PlayerCharacter)
	{
		if(!PlayerCharacter->IsBeingPickedUp())
		{
			BoxCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			GetWorldTimerManager().SetTimer(MovementUpTimer, this, &ABunnyMachine::MoveUpwards, TimeUntilDrop - 0.5f);
			GetWorldTimerManager().SetTimer(MovementDownTimer, this, &ABunnyMachine::MoveDownwards, TimeUntilDrop);
			GetWorldTimerManager().SetTimer(DestroyTimer, this, &ABunnyMachine::DestroyCarrot, (TimeUntilDrop + TimeUntilReset) + 0.5f);
			bHitCarrot = true;

			

			// Wwise 
			// Start rumble audio
			CarrotAudio->PostAkEvent(WEStartRumble);
		}
	}
}

void ABunnyMachine::OnOverlapBeginBunny(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp == CarrotMeshComponent)
	{
		NiagaraComponent->Activate();
		bIsEating = true;
	}
}

void ABunnyMachine::OnOverlapEndBunny(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherComp == CarrotMeshComponent)
	{
		NiagaraComponent->Deactivate();
		bIsEating = false;
	}
}

void ABunnyMachine::MoveDownwards()
{
	//Save the Location the Carrot is supposed to move to
	FVector TargetLocation = FVector(0.0, 0.0, DistanceToMoveDown);

	EMoveComponentAction::Type MoveActionFlag = EMoveComponentAction::Move;
	FLatentActionInfo LatenInfo;
	LatenInfo.CallbackTarget = this;
	
	//Move Carrot to Desired Location in x Seconds
	UKismetSystemLibrary::MoveComponentTo(CarrotMeshComponent, TargetLocation, CarrotMeshComponent->GetComponentRotation(), false, false, TimeUntilReset, false, MoveActionFlag, LatenInfo);

	bMoveDown = true;
	bMoveUp = false;
}

void ABunnyMachine::MoveUpwards()
{
	bMoveUp = true;
}

void ABunnyMachine::DestroyCarrot()
{
	bHitCarrot = false;
	CarrotMeshComponent->SetWorldLocation(InitialLocation + ResetOffset);
	NoShakeCollisionComponent->SetWorldLocation(InitialComponentLocation + ResetOffset);

	BoxCollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	bReset = true;

	CarrotRingMeshComponent->SetMaterial(0, MaterialsRing[0]);

	// Wwise 
	// Stop rumble audio
	CarrotAudio->PostAkEvent(WEStopRumble);
}

