// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "CheckArea.generated.h"

UCLASS()
class DREAMBOTS_API ACheckArea : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckArea();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Scene, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* SceneComponent;

	/** BoxComponent responsible for checking collision with Player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BoxCollision, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* BoxCollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Values, meta = (AllowPrivateAccess = "true"))
		double NewZThresholdValue = 0;
};
