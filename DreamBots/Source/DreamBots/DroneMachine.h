// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DreamBotsCharacter.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkGameplayStatics.h"
#include "DroneMachine.generated.h"
UENUM()
enum CurrentPossesType
{
	Player,
	Drone,
	None
};
UCLASS()
class DREAMBOTS_API ADroneMachine : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADroneMachine();




	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* DroneMachineMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* CollisionMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USceneComponent* DroneMachineRoot = nullptr;
	void ChangePossession(APlayerController* player);
	void SetPlayer(ADreamBotsCharacter* player) { PlayerToPossess = player; };

	void DisableMachine() { IsDroneMachineActive = false; };
	UFUNCTION(BlueprintCallable)
		bool GetDroneMachineActivity() const { return IsDroneMachineActive; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Drone, meta = (AllowPrivateAccess = "true"))
		class ADroneBase* DroneToPossess = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Drone, meta = (AllowPrivateAccess = "true"))
		float MaxDelayToReturn = 1.f;
	// Called when the game starts or when spawned
	void RepossessToPlayer(APlayerController* controller);
protected:
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:

	UPROPERTY()
		ADreamBotsCharacter* PlayerToPossess = nullptr;

	// Enter event
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WEEnterDrone;

	// Exit event
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WEExitDrone;

	CurrentPossesType CurrentPossess = Player;
	bool IsDroneMachineActive = true;
	bool IsReturningToPlayerPossess = false;
};
