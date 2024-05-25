// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SplineComponent.h"
#include "MountableActor.h"

// Sets default values
AMovingActor::AMovingActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// setup root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	Spline->SetupAttachment(Root);
	Spline->Duration = TotalPathTimeController;
	Spline->bDrawDebug = true;
}

// Called when the game starts or when spawned
void AMovingActor::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(true);
	if (ActorToMoveClass)
	{
		ActorToMove = GetWorld()->SpawnActor<AActor>(ActorToMoveClass, Spline->GetComponentTransform());
		if (ActorToMove)
		{
			//Time when the press play
			bCanMoveActor = true;
		}
	}

	if (AMountableActor* actor = Cast<AMountableActor>(ActorToMove))
	{
		actor->SetMovingActor(this);
	}

	DelayWaitingTime = MaxDelayWaitingTime;
	if (DockingPoint < 0)
	{
		bEnableDocking = false;
	}
}

// Called every frame
void AMovingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//If actor is at the docking area, stop movement and set at the docking area to true.
	if (!bAtTheDockingArea && bEnableDocking && bCanDock)
	{
		auto SplinePoint = Spline->GetLocationAtSplinePoint(DockingPoint, ESplineCoordinateSpace::World);
		if (SplinePoint.Equals(ActorToMove->GetActorLocation(), 100.f))
		{
			bCanMoveActor = false;
			bAtTheDockingArea = true;
			bCanDock = false;
		}
	}
	//if actor can't move start a timer for it to continue moving afterwards.
	if (!bCanMoveActor)
	{
		if (DelayWaitingTime <= 0.f)
		{
			bCanMoveActor = true;
			bAtTheDockingArea = false;
			DelayWaitingTime = MaxDelayWaitingTime;
		}
		else
		{
			DelayWaitingTime -= DeltaTime;
		}
	}

	if (ActorToMove && bCanMoveActor)
	{
		CurrentSplineTime += DeltaTime * Speed;

		float CurrentSplineLoop{ CurrentSplineTime / TotalPathTimeController };

		float Distance = Spline->GetSplineLength() * CurrentSplineLoop;

		//World position
		FVector Position = Spline->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
		ActorToMove->SetActorLocation(Position);

		//World rotation
		FVector Direction = Spline->GetDirectionAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);

		//create rotator given a vector director (normalized)
		FRotator Rotator = FRotationMatrix::MakeFromX(Direction).Rotator();
		ActorToMove->SetActorRotation(Rotator);
	
		//Reach the end
		if (CurrentSplineLoop >= 1.f)
		{
			if (bSplineInLoop)
			{
				bCanMoveActor = true;
				CurrentSplineTime = 0.f;
				if (bEnableDocking)
					bCanDock = true;
			}
		}
	}
}

