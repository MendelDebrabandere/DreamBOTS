// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkGameplayStatics.h"
#include "GrabbingMachine.generated.h"

UCLASS()
class DREAMBOTS_API AGrabbingMachine : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrabbingMachine();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		bool GetIsGrab() { return bGrabbed; };

	UFUNCTION(BlueprintCallable)
		bool GetIsClosed() { return bClose; };

	UFUNCTION(BlueprintCallable)
		bool GetCanGrabPlayer() { return bDoesGrabPlayer; };

	UFUNCTION(BlueprintCallable)
	void SetClosed(bool condition) { bClose = condition; };

	UFUNCTION(BlueprintCallable)
	void SetCanGrabPlayer(bool condition) { bDoesGrabPlayer = condition; };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Values, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ArmExtender;

	UPROPERTY(BlueprintReadWrite)
	bool bShouldSetVertSize{ true };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Scene, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* SceneComponent;

	/** Static Mesh which is editable -> Should contain GrabMachine */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SkeletalMesh, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* SkeletalMeshComponent;

	/** BoxComponent responsible for checking collision with Player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BoxCollision, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* BoxCollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Values, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* WayPointUp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Values, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* WayPointSideway;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Anim, meta = (AllowPrivateAccess = "true"))
		class UAnimSequence* AnimSequence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Decal, meta = (AllowPrivateAccess = "true"))
		class UDecalComponent* Decal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WEGrab;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WERelease;

	UFUNCTION()
		void MoveMachineUp();

	UFUNCTION()
		void MoveMachineSideway();

	UFUNCTION()
		void ReleasePlayer(ADreamBotsCharacter* PlayerCharacter);

	UFUNCTION()
		void DisableGrab();

	UFUNCTION()
		void MoveToInitalPos();

	UFUNCTION()
		void Idle();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Values, meta = (AllowPrivateAccess = "true"))
		bool bHorizontalInX = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Values, meta = (AllowPrivateAccess = "true"))
		bool bHorizontalInY = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Values, meta = (AllowPrivateAccess = "true"))
		bool bVertical = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Values, meta = (AllowPrivateAccess = "true"))
		double Amplitude = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Values, meta = (AllowPrivateAccess = "true"))
		double Speed = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Values, meta = (AllowPrivateAccess = "true"))
		double WorldZGrab = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Values, meta = (AllowPrivateAccess = "true"))
		bool bDoesGrabPlayer = true;

	FVector InitialLocation{};

	UAnimInstance* AnimInstance{};

	bool bGrabbed = false;
	bool bClose = false;
	bool bInitHorX = false;
	bool bInitHorY = false;
	bool bInitVert = false;

	float bTime = 0.0f;

	FTimerHandle MovementTimer{}, MovementTimerSide{}, GrabTimer{}, MoveInitTimer{}, IdleTimer{};

	FTimerDelegate ReleasePlayerDelegate{};
};
