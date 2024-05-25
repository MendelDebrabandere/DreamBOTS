// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrabbingMachine.h"
#include "Components/BoxComponent.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkGameplayStatics.h"
#include "ToyBall.generated.h"

UCLASS()
class DREAMBOTS_API AToyBall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AToyBall();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void MoveUpwards();

	UFUNCTION(BlueprintImplementableEvent)
	void HideGrabber(bool SetHide);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	/** Static Mesh which is editable -> Should contain ToyBallMesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* ToyBallMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Grabber, meta = (AllowPrivateAccess = "true"))
		class UChildActorComponent* GrabberComponent;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Grabber, meta = (AllowPrivateAccess = "true"))
		//class AGrabbingMachine* GrabbingMachineClass;

	/** BoxComponent responsible for checking collision with Player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BoxCollision, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* BoxCollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkComponent* BallAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WERise;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WEGrab;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Values, meta = (AllowPrivateAccess = "true"))
		double DistanceToMoveUp = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Values, meta = (AllowPrivateAccess = "true"))
		bool TriggeredByPlayer = true;

	FTimerHandle DelayMoveTimer{};

	UPROPERTY()
	AGrabbingMachine* GrabberActor{};

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bPickUpBall{};

};
