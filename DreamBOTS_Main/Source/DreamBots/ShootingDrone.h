// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DroneBase.h"
#include "Components/ArrowComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "InputActionValue.h"
#include "DroneBullet.h"
#include "Wasp.h"
#include "ShootingDrone.generated.h"

/**
 *
 */
UCLASS()
class DREAMBOTS_API AShootingDrone : public ADroneBase
{
	GENERATED_BODY()
public:
	AShootingDrone();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Firing)
		UNiagaraComponent* NiagaraComponentLeft = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Firing)
		UNiagaraComponent* NiagaraComponentRight = nullptr;

	UFUNCTION()
		void ShootProjectile();

	UFUNCTION()
		void StartShootProjectileNiagara();

	UFUNCTION()
		void StopShootingProjectile();

	UFUNCTION()
		void ReturnToPlayer();

	UFUNCTION()
		void CheckWasps();
	UFUNCTION()
		void AutoAim(float deltaTime);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Firing)
		UNiagaraSystem* NiagaraSystem = nullptr;

	// Projectile class to spawn.
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = Projectile)
		TSubclassOf<class ADroneBullet> ProjectileClass;

	// Projectile class to spawn.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Wasp)
		TSubclassOf<class AWasp> WaspClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
		float MaxTiming = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Aiming)
		float InterpSpeed = 5.f;
protected:
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	UAkComponent* PoisonAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	UAkAudioEvent* WEShoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	UAkAudioEvent* WEStopShoot;

	float CurrentTiming = 0.f;
	TArray<AActor*> FoundActors;
	float ClosestWasp = 0.f;

	int ClosestWaspIdx = 0.f;
	bool HasSpawnMovieTapeOnce = false;
	bool IsWaspChecked = false;
};
