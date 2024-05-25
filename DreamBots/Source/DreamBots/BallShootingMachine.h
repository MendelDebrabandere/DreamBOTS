// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cannonball.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkGameplayStatics.h"

#include "BallShootingMachine.generated.h"

class UBoxComponent;

UCLASS()
class DREAMBOTS_API ABallShootingMachine : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABallShootingMachine();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void ShootingCooldown(float DeltaTime);
	void ShootBall(int index);
	void RotateCanons();
	void UpdateShootTimings();
	void LerpCannonRotations();

	FTimerHandle LerpTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Tower1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Tower2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Tower1Cog;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Tower2Cog;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Tower1Cannon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Tower2Cannon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Crystal1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Crystal2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Crystal1Cannon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Crystal2Cannon;

	UPROPERTY(EditAnywhere, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* SmokeTower1Component{};

	UPROPERTY(EditAnywhere, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* SmokeTower2Component{};

	UPROPERTY(EditAnywhere, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* SmokeCrystal1Component{};

	UPROPERTY(EditAnywhere, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* SmokeCrystal2Component{};

	UNiagaraComponent* EffectsArray[4]{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ACannonball> CannonballClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ACannonball> TennisballClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ACannonball> HockeyballClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	UAkAudioEvent* WEShoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	UAkComponent* MovingAudio;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* ShootingZone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music", meta = (AllowPrivateAccess = "true"))
	class UAkStateValue* MusicState;

	static constexpr int NrOfCannons{ 4 };
	int ShootingCooldownsArr[NrOfCannons]{};

	UPROPERTY(EditAnywhere, Category = "ShootingVaribales")
	int MinShootingCooldown{ 1 };

	UPROPERTY(EditAnywhere, Category = "ShootingVaribales")
	int MaxShootingCooldown{ 4 };

	UPROPERTY(EditAnywhere, Category = "ShootingVaribales")
	float ShootingForce{ 3000000 };

	UPROPERTY(EditAnywhere, Category = "ShootingVaribales")
	double ShotDelay{ 1.f };

	UPROPERTY(EditAnywhere, Category = "CannonRotation")
	int LerpSpeed{ 6 };

	UPROPERTY(EditAnywhere, Category = "CannonRotation")
	double TimeToRotate { 2.f };

	double CurrTimeToRotate{ 0.0 };

	UPROPERTY(EditAnywhere, Category = "CannonRotation")
	int RotationIncrement{ 20 };

	UPROPERTY(EditAnywhere, Category = "CannonRotation")
	int MaxRotationIncrement{ 20 };

	UPROPERTY(EditAnywhere, Category = "Music")
	int BPM{ 134 };

	UPROPERTY(EditAnywhere, Category = "Music")
	double FirstBeatOffset{ 0.242 };

	double SecPerBeat{ 0.0 };
	double CurShotDelay{ 0.0 };

	int RotationDirection{ 1 };

	int CurrRotation{};


	UStaticMeshComponent* CannonMeshes[NrOfCannons]{};
	FRotator GearSpawnRotations[NrOfCannons]{};
	FRotator TargetCannonRotations[NrOfCannons]{};



	UFUNCTION()
	void OnBeginOverlapForShootingZone(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlapForShootingZone(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool bPlayerIsInZone{};

};
