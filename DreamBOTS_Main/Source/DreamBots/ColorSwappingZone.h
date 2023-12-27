// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DreamBotsCharacter.h"
#include "ColorSwappingZone.generated.h"

UCLASS()
class DREAMBOTS_API AColorSwappingZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AColorSwappingZone();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	//Root
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USceneComponent* Root = nullptr;

	// Box of the trigger
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* BoxComp = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color", meta = (AllowPrivateAccess = "true"))
		TEnumAsByte<CharacterSkinColor> NewColor;

	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
