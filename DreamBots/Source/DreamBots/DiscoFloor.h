// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkGameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "Components/BoxComponent.h"
#include "DiscoFloor.generated.h"

UCLASS()
class DREAMBOTS_API ADiscoFloor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADiscoFloor();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	void SwapToNextTexture();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	AStaticMeshActor* ToyBallMachine;

	UPROPERTY()
	USceneComponent* Root{};

	UPROPERTY(EditAnywhere)	
	class UBoxComponent* StartBox{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State", meta = (AllowPrivateAccess = "true"))
	class UAkStateValue* State;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State", meta = (AllowPrivateAccess = "true"))
	FName StateGroupName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State", meta = (AllowPrivateAccess = "true"))
	FName StateName;

	UPROPERTY(EditAnywhere, Category = "Music")
	int BPM{ 166 };

	UPROPERTY(EditAnywhere, Category = "Music")
	unsigned int TriggerPerBeat{ 2 };

	UPROPERTY(EditAnywhere, Category = "Music")
	unsigned int DelayBeat{ 1 };

	UPROPERTY(EditAnywhere, Category = "Material")
	int DiscoMaterialIndex{ 7 };

	UPROPERTY(EditAnywhere, Category = "Material")
	TArray<UTexture*> DiscoTextures{};

	UMaterialInstanceDynamic* DynamicMatierial{};
	unsigned int TexureIndex{ 0 };
	bool bMusicIsPlaying{ false };
	double SecPerBeat{ 1.0 };
	double SecPerTextureChange{ 0.0 };
	double CurrentSecToChangeTexture{ 0.0 };
};
