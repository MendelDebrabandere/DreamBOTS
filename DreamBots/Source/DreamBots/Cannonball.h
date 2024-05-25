// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkGameplayStatics.h"
#include "Cannonball.generated.h"

UCLASS()
class DREAMBOTS_API ACannonball : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACannonball();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Call this function to shoot the cannonball
	void Shoot(FVector Direction, float Force);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes", meta = (AllowPrivateAccess = "true"))
	//float Mass{1000};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lifetime", meta = (AllowPrivateAccess = "true"))
	float MaxLifeTime{ 13.f };
	float CurrLifeTime{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "KnockBack", meta = (AllowPrivateAccess = "true"))
	float KnockBackForce{ 1000.f };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "KnockBack", meta = (AllowPrivateAccess = "true"))
	float VerticalKnockBack{ 0.3f };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "KnockBack", meta = (AllowPrivateAccess = "true"))
	float HorizontalSpeed{ 500.f };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	UAkComponent* BallAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	double PhysicsAudioTreshhold = 25000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	double PhysicsAudioMax = 50000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	UAkAudioEvent* WETail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	UAkAudioEvent* WEPlayerImpact;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	UAkAudioEvent* WEPhysicImpact;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	UAkRtpc* RTPCVelocity;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	float newCannonBallSize{};
};
