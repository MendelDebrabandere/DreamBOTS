// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DroneBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkGameplayStatics.h"
#include "LegoDrone.generated.h"

/**
 * 
 */
class ALegoPiece;

UCLASS()
class DREAMBOTS_API ALegoDrone : public ADroneBase
{
	GENERATED_BODY()
public:
	ALegoDrone();

	virtual void Tick(float DeltaTime) override;

	void SetLegoAttach(bool bAttach);
	bool AttachStatus();
	void IncreasePiecesPlaced() { ++PiecesPlaced; };
	double GetZOffset() { return ZOffset; };
	UBoxComponent* GetPBoxComponent() { return PickUpBoxComponent; };

protected:
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* PickUpBoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Drone, meta = (AllowPrivateAccess = "true"))
		class ADroneMachine* DroneMachine;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* ArrowMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Objectives, meta = (AllowPrivateAccess = "true"))
		TArray<ALegoPiece*> LegoPieces{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Values, meta = (AllowPrivateAccess = "true"))
		double LerpValue = { 0.05 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Values, meta = (AllowPrivateAccess = "true"))
		double ZOffset = { 100.0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WELegoPickedup;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WELegoReleased;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkComponent* MagnetAudio;

	bool bAttached = false;

	int PiecesPlaced{ 0 };

	ALegoPiece* ClosestPiece{};

	FVector TargetLocation{};
		
};
