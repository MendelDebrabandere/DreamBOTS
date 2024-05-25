// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ToyBall.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkGameplayStatics.h"
#include "MazeConsole.generated.h"

UCLASS()
class DREAMBOTS_API AMazeConsole : public AActor
{
	GENERATED_BODY()


public:
	// Sets default values for this actor's properties
	AMazeConsole();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* ToyballConsoleMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* CollisionMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USceneComponent* ToyballConsoleRoot = nullptr;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class AToyBall> ToyballClass;


	void DisableMachine() { IsToyballConsoleActive = false; };
	UFUNCTION(BlueprintCallable)
		bool GetMazeConsoleActivity() const { return IsToyballConsoleActive; }
	void ActivateToyBalls();
protected:
	// Called when the game starts or when spawned

	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	UAkAudioEvent* WEActivate;

	bool IsToyballConsoleActive = true;
	TArray<AActor*> FoundActors;

};
