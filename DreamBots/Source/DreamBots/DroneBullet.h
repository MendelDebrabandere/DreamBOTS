// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Wasp.h"
#include "DroneBullet.generated.h"

UCLASS()
class DREAMBOTS_API ADroneBullet : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADroneBullet();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* DroneBulletRoot = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USphereComponent* CollisionMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UProjectileMovementComponent* ProjectileComponent = nullptr;

	void FireProjectile(FVector LaunchDirection);

	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:


};
