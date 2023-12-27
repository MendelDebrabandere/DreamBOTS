// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <vector>
#include "MovingActor.generated.h"

UCLASS()
class DREAMBOTS_API AMovingActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMovingActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void SetSpeed(float speed) { Speed = speed; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		int DockingPoint = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float MaxDelayWaitingTime = 2.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleAnywhere)
		USceneComponent* Root {};

	UPROPERTY(VisibleAnywhere)
		class USplineComponent* Spline{};

	UPROPERTY(EditAnywhere, category = "Settings")
		TSubclassOf<AActor> ActorToMoveClass{};

	UPROPERTY(VisibleAnywhere)
		class AActor* ActorToMove{};

	UPROPERTY(EditAnywhere, category = "Settings")
		float TotalPathTimeController;

	UPROPERTY(EditAnywhere, category = "Settings")
		bool bSplineInLoop;

	bool bCanMoveActor;
	bool bAtTheDockingArea = false;
	float DelayWaitingTime = 2.f;
	bool bEnableDocking = true;
	bool bCanDock{ true };
	float Speed{ 1.f };

	float CurrentSplineTime{};
};
