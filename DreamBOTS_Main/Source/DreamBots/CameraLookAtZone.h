// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraLookAtZone.generated.h"

class UBoxComponent;

UCLASS()
class DREAMBOTS_API ACameraLookAtZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraLookAtZone();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "CameraSettings")
	bool LookAtInFrontOfPlayer{true};

	UPROPERTY(EditAnywhere, Category = "CameraSettings")
	float BoomArmPitch{-15.f};

	UPROPERTY(EditAnywhere, Category = "CameraSettings")
	float CameraPitch{ 5.f };

	UPROPERTY(EditAnywhere, Category = "CameraSettings")
	float CameraBoomLength{ 450.f };

	UPROPERTY(EditAnywhere, Category = "CameraSettings")
	float CameraTransitionSpeed{ 1.5f };

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* LookAtPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxCollision;

};
