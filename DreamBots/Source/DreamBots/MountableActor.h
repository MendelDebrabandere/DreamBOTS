// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MovingActor.h"
#include "MountableActor.generated.h"

UCLASS()
class DREAMBOTS_API AMountableActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMountableActor();

	void SetMovingActor(AMovingActor* actor) { MovingActor = actor; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USceneComponent* RideRoot = nullptr;

	UFUNCTION(BlueprintImplementableEvent, Category = "Event") // BP Event
		void Mounted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Event") // BP Event
		void Dismount();

	UFUNCTION(BlueprintCallable)
		AMovingActor* GetMovingActor() const { return MovingActor; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Overlap function

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		USceneComponent* Root = nullptr;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* CollisionBox = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (AllowPrivateAccess = "true"))
		class UNiagaraComponent* FireParticles{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* SkeletalMesh = nullptr;

	AMovingActor* MovingActor{ nullptr };
};
