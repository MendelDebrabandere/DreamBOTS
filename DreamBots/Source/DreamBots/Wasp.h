// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkGameplayStatics.h"
#include "Wasp.generated.h"

UCLASS()
class DREAMBOTS_API AWasp : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWasp();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USceneComponent* WaspRoot = nullptr;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBoxComponent* CollisionBox = nullptr;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Niagara)
		UNiagaraComponent* NiagaraComponent = nullptr;

	UFUNCTION()
		void GetHit();
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int MaxHealth = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int CurrentHealth = 0;
	//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHitSignature,bool, IsWaspHit);
	//UPROPERTY(BlueprintAssignable);
	//FOnHitSignature IsHit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float NiagaraStartSpawnRate = 200.f;
	static int GetWaspCounter() { return WaspCounter; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


private:
	UPROPERTY(EditAnywhere, Category = Firing)
		UNiagaraSystem* NiagaraSystem = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkComponent* WaspAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WEHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WEIdle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WEDeath;

	FVector NiagaraScale;
	float HealthNiagaraSpawnRate{};
	inline static int WaspCounter{};
};
