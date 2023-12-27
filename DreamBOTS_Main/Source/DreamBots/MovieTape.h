// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MovieTape.generated.h"

UENUM()
enum DrawingType
{
	GeldDrone,
	WespWaterDrone,
	Legobouwer,
	Balmachine,
	VliegendeBoot,
	KonijnetenMachine,
	Kleermaker,
	Snoepkraan,
	MagicGrijper,
	EtenRobot,
	TakenRobot,
	Vrago,
	Matti,
	Kippie,
	Deleuker,
	DeAllesRobot
};

UCLASS()
class DREAMBOTS_API AMovieTape : public AActor
{
	GENERATED_BODY()

public:

	// Sets default values for this actor's properties
	AMovieTape();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* MovieTapeMesh = nullptr;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DrawingVariable")
	//	int MovieTapeIdx = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
		float RotationSpeed = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bobbing")
		float BobbingSpeed = 2.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bobbing")
		float TotalTime = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bobbing")
		float HeightDisplacement = 0.75f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScaleAnim")
		float Amplitude = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScaleAnim")
		float Frequency = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScaleAnim")
		float MaxScale = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScaleAnim")
		float MinScale = 1.8f;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drawing")
	//	int MovieTapeNumber = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drawing")
		TEnumAsByte<DrawingType>  MovieTapeEnum = GeldDrone;
	UFUNCTION()
		void RotateMovieTape(float elapsed);
	UFUNCTION()
		void ScaleAnimation(float elapsed);
	UFUNCTION()
		void DestroyTape();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
private:
	UBoxComponent* CollisionMesh = nullptr;
	FRotator RotationDirection = { 0.f,180.f,0.f };
	float TimeAccumulator = 0.0f;
	bool bCollected = false;
	FTimerHandle DestroyTimer{};


};
