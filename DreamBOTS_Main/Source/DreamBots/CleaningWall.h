// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CleaningWall.generated.h"

UCLASS()
class DREAMBOTS_API ACleaningWall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACleaningWall();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetBlackPercentage(int percentage, int destroyPercentage);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	UPROPERTY()
	USceneComponent* Root{};

	FVector SpawnScale{};
	FVector TargetScale{};
	FVector CurrentScale{};

	void DestroyObject() { Destroy(); };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* WallMesh{};

};
