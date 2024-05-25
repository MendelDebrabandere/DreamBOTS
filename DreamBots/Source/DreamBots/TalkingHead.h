// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkGameplayStatics.h"
#include "TalkingHead.generated.h"

UCLASS()
class DREAMBOTS_API ATalkingHead : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATalkingHead();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void StartTalking() const;
	void StopTalking() const;
	void StartFly() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void MoveHeadTowardsPlayer(float dtime);
	void SineWaveHeadBobbing(float dtime);


	UPROPERTY()
	USceneComponent* Root{};

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* HeadPivot{};

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* HeadMesh{};

	UPROPERTY(EditAnywhere)
	class UBoxComponent* StartFollowBox{};

	UPROPERTY(EditAnywhere)
	class UBoxComponent* EndFollowBox{};

	UPROPERTY(EditAnywhere)
	USceneComponent* EndPoint{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	class UAkComponent* HeadAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	class UAkAudioEvent* WEStartTalking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	class UAkAudioEvent* WEStartFlying;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	class UAkAudioEvent* WEStopTalking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	class UAkAudioEvent* WEStopFlying;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	class UAkRtpc* RtpcSpeed;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	bool bShouldFollow{};

	UPROPERTY()
	class ADreamBotsCharacter* Character{};

	UPROPERTY(EditAnywhere)
	float FollowDistance{ 300 };

	UPROPERTY(EditAnywhere)
	float EngineMaxSpeed{ 350.f };

	UPROPERTY(EditAnywhere)
	float HorBobbing{ 55 };
	UPROPERTY(EditAnywhere)
	float VertBobbing{ 25 };
	UPROPERTY(EditAnywhere)
	float BobbingSpeed{ 1.5f };
	float CurrBobbingSpeed{ 0.f };
	float SinePosition{};

	//Scaling animation
	double StartTime{};
	float Duration{};
	FTimerHandle ScaleTimerHandle{};
	FTimerHandle MuteTimerHandle{};

	void ScaleMeshOverTime();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TalkingHeadMesh", meta = (AllowPrivateAccess = "true"))
	AActor* SceneHeadMesh;

};
