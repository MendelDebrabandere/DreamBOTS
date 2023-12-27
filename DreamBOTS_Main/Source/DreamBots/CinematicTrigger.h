// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "LevelSequenceActor.h"
#include "CinematicTrigger.generated.h"

UCLASS()
class DREAMBOTS_API ACinematicTrigger : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACinematicTrigger();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		ALevelSequenceActor* Cinematic = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		ULevelSequence* LevelSequencer = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool IsEndSequence = false;
	//On overlap events
	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	bool CinematicHasBeenPlayed{};

	UFUNCTION()
	void OnCinematicFinished();

	// Collisionbox of the trigger
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* CollisionMesh = nullptr;

	//New root of the class
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USceneComponent* TriggerRoot = nullptr;

	UPROPERTY()
	class ADreamBotsCharacter* Character{};
};
