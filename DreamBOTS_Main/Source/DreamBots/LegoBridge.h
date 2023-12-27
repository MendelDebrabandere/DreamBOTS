// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkGameplayStatics.h"
#include "LegoBridge.generated.h"

UCLASS()
class DREAMBOTS_API ALegoBridge : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALegoBridge();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UStaticMeshComponent* GetCurrentLegoMesh() { return LegoPieceMesh; };
	UBoxComponent* GetPlaceLegoPieceBox() { return PlaceLegoPieceBox; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Scene, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* LegoPieceMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* PlaceLegoPieceBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Material, meta = (AllowPrivateAccess = "true"))
		TArray<UMaterialInstance*> MaterialsBridge;

	UPROPERTY()
		TArray<UMaterialInstanceDynamic*> DynamicMaterialsBridge{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WELegoPlaced;

	//UMaterialInstanceDynamic* MaterialInstance = nullptr;

};
