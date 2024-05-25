// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "InputActionValue.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "DreamBotsCharacter.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkGameplayStatics.h"
#include "DroneMachine.h"
#include "MovieTape.h"
#include "Engine/TargetPoint.h"
#include "DroneBase.generated.h"

UCLASS()
class DREAMBOTS_API ADroneBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADroneBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* DroneRoot = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* CollisionMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UFloatingPawnMovement* FloatingPawnMovement;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* MoveAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* InteractAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		class UAkComponent* DroneAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		class UAkAudioEvent* WEStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		class UAkRtpc* RtpcDroneSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* DroneMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* DroneAttachment = nullptr;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);

	void Interact(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = Drone)
	void SetDroneMachine(ADroneMachine* droneMachine) {DroneMachine = droneMachine;	};

	UFUNCTION(BlueprintCallable, Category = Drone)
	ADroneMachine* GetDroneMachine() const {return DroneMachine;};

	void SetCameraLookAtPoint(USceneComponent* lookAtPoint, bool inFront, float boomArmPitch, float cameraPitch, float boomArmLength, float transitionSpeed);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Drone)
		float MaxRange = 2400.f;

	bool IsOutOfRange();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	ADroneMachine* DroneMachine = nullptr;
	virtual void ReturnToInitialSpot();

	void ResetDroneSpeedRPTC();

private:
	// PlayerController
	APlayerController* PlayerController = nullptr;

	void UpdateDroneSpeedRPTC();

	//Camera
	void DoCameraLookAtLogic();

	USceneComponent* CameraLookAtPoint{};

	bool bCameraLookAtInFront{};

	float BoomArmPitchOffset{}, CameraPitchOffset{}, BoomArmLength{};

	FRotator CurrBoomArmRotation{};
	FVector StartingPosition{};
	FRotator StartingRotation{};

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraTransitionSpeed{ 1.5f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Values, meta = (AllowPrivateAccess = "true"))
		float TiltX{ 10.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Values, meta = (AllowPrivateAccess = "true"))
		float TiltY{ 10.f };
};
