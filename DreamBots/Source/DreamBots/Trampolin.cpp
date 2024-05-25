// Fill out your copyright notice in the Description page of Project Settings.


#include "Trampolin.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkAudioEvent.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkRtpc.h"
#include "NiagaraComponent.h"

// Sets default values
ATrampolin::ATrampolin()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create Scene Component
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SceneComponent->SetupAttachment(RootComponent);

	TrampolinMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrampolinMesh"));
	TrampolinMeshComponent->SetupAttachment(SceneComponent);

	TrampolineFloorMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TrampolinFloor"));
	TrampolineFloorMesh->SetupAttachment(SceneComponent);

	BoxCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollisionComponent->SetupAttachment(TrampolinMeshComponent);

	// Smoke
	SmokeComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SmokeComponent"));
	SmokeComponent->SetupAttachment(SceneComponent);
	SmokeComponent->bAutoActivate = false;
}

// Called when the game starts or when spawned
void ATrampolin::BeginPlay()
{
	Super::BeginPlay();
	
	if (MaterialsTrampolin.Num() == 2)
	{
		DynamicMaterialTrampolin = UMaterialInstanceDynamic::Create(MaterialsTrampolin[1], this);
	}

	BoxCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ATrampolin::OnOverlapBegin);
	BoxCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ATrampolin::OnEndOverlap);
	UAkGameplayStatics::SetRTPCValue(RtpcHeight, LaunchVector.Z, 0, this, "RTPC_Trampoline_height");
}

void ATrampolin::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PlayerCharacter = Cast<ADreamBotsCharacter>(OtherActor);

	if(PlayerCharacter)
	{
		PlayerCharacter->SetTrampolinActor(this);
		PlayerCharacter->JumpedOnTrampoline(true);
		PlayerCharacter->LaunchCharacter(LaunchVector, true, true);

		if(!MaterialsTrampolin.IsEmpty())
		{
			TrampolinMeshComponent->SetMaterial(0, MaterialsTrampolin[1]);
			GetWorldTimerManager().SetTimer(ChangeMaterialTimer, this, &ATrampolin::ChangeToInitMaterial, 0.5f, false);
		}

		bPlayAnimation = true;

		// Wwise
		// Jump audio
		FOnAkPostEventCallback nullCallback{};
		WEJump->PostOnActor(this, nullCallback, 0, false);

		if (SmokeComponent)
		{
			SmokeComponent->ActivateSystem();
		}
	}
}

void ATrampolin::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bPlayAnimation = false;
}

void ATrampolin::ChangeToInitMaterial()
{
	TrampolinMeshComponent->SetMaterial(0, MaterialsTrampolin[0]);
}

// Called every frame
void ATrampolin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}





