// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "DreamBotsCharacter.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AddonPickup.generated.h"

UCLASS()
class DREAMBOTS_API AAddonPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAddonPickup();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Settings")
	UStaticMeshComponent* AddonMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Settings")
	UBoxComponent* CollisionMesh = nullptr;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float RotationSpeed = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool ShouldDisappear{};

	UFUNCTION()
	void RotatePickup(float elapsed);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere)
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, Category = "Settings")
	TEnumAsByte<CharacterType> Type{};

	FRotator RotationDirection = { 0.f,180.f,0.f };
};
