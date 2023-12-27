// Fill out your copyright notice in the Description page of Project Settings.


#include "PoisonFloor.h"

#include "DreamBotsCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/TextureRenderTarget2D.h"
#include "CanvasTypes.h"
#include "Engine/Canvas.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MovieTape.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APoisonFloor::APoisonFloor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//setup Root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	MovieTapeSpawnSpot = CreateDefaultSubobject<USceneComponent>(TEXT("MovieTapeSpawnSpot"));
	MovieTapeSpawnSpot->SetupAttachment(Root);

	//setup raycastfloor
	RaycastFloor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RaycastFloor"));
	RaycastFloor->SetupAttachment(RootComponent);
	RaycastFloor->SetCollisionProfileName(TEXT("Vehicle"));

	//setup boxoverlap
	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
	OverlapBox->SetupAttachment(RootComponent);

	//setup poisonDecal
	PoisonDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("PoisonDecal"));
	PoisonDecal->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APoisonFloor::BeginPlay()
{
	Super::BeginPlay();

	//setup overlap
	OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &APoisonFloor::OnBeginOverlap);
	OverlapBox->OnComponentEndOverlap.AddDynamic(this, &APoisonFloor::OnEndOverlap);

	//create dynamic material
	if (RaycastFloor && DrawingMaterial)
	{
		DynamicDrawingMaterial = UMaterialInstanceDynamic::Create(DrawingMaterial, this);
		RaycastFloor->SetMaterial(0, DynamicDrawingMaterial);
	}

	//Clear rendertarget is done in the blueprint, it is way easier and better supported in blueprint


	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APoisonFloor::CheckRenderTargetBlackPercentage, 0.1f, true);

}

void APoisonFloor::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADreamBotsCharacter* character = Cast<ADreamBotsCharacter>(OtherActor);

	if (character)
	{
		Character = character;
		bCharacterInZone = true;
	}
}

void APoisonFloor::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ADreamBotsCharacter* character = Cast<ADreamBotsCharacter>(OtherActor);

	if (character)
	{
		bCharacterInZone = false;
	}
}

void APoisonFloor::DrawToRT(const FVector2D& pos)
{
	DynamicDrawingMaterial->SetVectorParameterValue(FName("DrawLocation"), FLinearColor(pos.X, pos.Y, 0.1f, 0.f));
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, PoisonRenderTarget, DynamicDrawingMaterial);
}

void APoisonFloor::CheckRenderTargetBlackPercentage()
{
	if (bCleaned == true)
		return;

	if (bCharacterInZone && Character && Character->GetCharType() == Cleaning && MovieTapeClass && MyRenderTarget)
	{
		// Capture variables required for the render command
		UTextureRenderTarget2D* LocalRenderTarget = MyRenderTarget;

		TArray<FColor> OutBMP;
		FReadSurfaceDataFlags ReadPixelFlags(RCM_UNorm);
		ReadPixelFlags.SetLinearToGamma(false);

		// Get the render target resource
		if (!PoisonRenderTargetResource)
		{
			PoisonRenderTargetResource = LocalRenderTarget->GameThread_GetRenderTargetResource();
		}

		if (PoisonRenderTargetResource)
		{
			// Read the pixels on the render thread
			PoisonRenderTargetResource->ReadPixels(OutBMP, ReadPixelFlags);
		}

		const float Step = 2;
		float BlackPixelCount = 0;

		// Sample pixels across the render target
		for (int32 i = 0; i < OutBMP.Num(); i += Step)
		{
			BlackPixelCount += OutBMP[i] == FColor::Black;
		}

		int BlackPercentage = static_cast<int>(BlackPixelCount / (OutBMP.Num() / Step) * 100); // Adjust the percentage based on the step

		//Update the walls
		for (ACleaningWall* Wall : Walls)
		{
			if(Wall)
				Wall->SetBlackPercentage(BlackPercentage, DestroyPercentage);
		}

		//Toggle audio, bubbles and rotation
		if (!bCleaning && PercentageFilled != BlackPercentage)
		{
			bCleaning = true;
			Character->StartCleaning();
		}
		else if (bCleaning && PercentageFilled == BlackPercentage)
		{
			bCleaning = false;
			Character->StopCleaning();
		}

		PercentageFilled = BlackPercentage;

		if (BlackPercentage <= CleanPercentage)
		{
			bCleaned = true;
			Character->StopCleaning();
			ClearFloor();
		}
	}
}

void APoisonFloor::ReadPixelsThreaded()
{

}

// Called every frame
void APoisonFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCharacterInZone && Character->GetCharType() == Cleaning)
	{
		Timer += DeltaTime;

		if (Timer >= 1 / LineTracesPerSecond)
		{
			Timer -= 1 / LineTracesPerSecond;

			DoCharacterLineTrace();
		}
	}
}

