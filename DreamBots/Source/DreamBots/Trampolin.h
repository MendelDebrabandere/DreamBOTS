// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "DreamBotsCharacter.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkGameplayStatics.h"
#include "Trampolin.generated.h"

UCLASS()
class DREAMBOTS_API ATrampolin : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrampolin();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool GetShouldAnimate() const {return bPlayAnimation;}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* TrampolinMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* TrampolineFloorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BoxCollision, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* BoxCollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Scene, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WEJump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		class UAkRtpc* RtpcHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Launch, meta = (AllowPrivateAccess = "true"))
		FVector LaunchVector {1000.0, 0.0, 1000.0};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MaterialTrampo, meta = (AllowPrivateAccess = "true"))
		TArray<UMaterialInstance*> MaterialsTrampolin;

	UPROPERTY(EditAnywhere, Category = "Effects", meta = (AllowPrivateAccess = "true"))
		class UNiagaraComponent* SmokeComponent{};

	UPROPERTY()
		UMaterialInstanceDynamic* DynamicMaterialTrampolin{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Values, meta = (AllowPrivateAccess = "true"))
		float EnableInputTime = 2.f;

	ADreamBotsCharacter* PlayerCharacter{};

	FTimerHandle ChangeMaterialTimer{};

	bool bPlayAnimation{};

	void ChangeToInitMaterial();


};
