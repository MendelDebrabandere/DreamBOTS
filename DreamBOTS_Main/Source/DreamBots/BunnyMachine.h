// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkGameplayStatics.h"
#include "BunnyMachine.generated.h"

UCLASS()
class DREAMBOTS_API ABunnyMachine : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABunnyMachine();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool IsEating() { return bIsEating; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapBeginBunny(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEndBunny(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	/** Static Mesh which is editable -> Should contain CarrotMesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* CarrotMeshComponent;

	/** Static Mesh which is editable -> Should contain CarrotRingMesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* CarrotRingMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BoxCollision, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* NoShakeCollisionComponent;

	/** BoxComponent responsible for checking collision with Player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BoxCollision, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* BoxCollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BoxCollision, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* BunnyBoxComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Scene, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MaterialRing, meta = (AllowPrivateAccess = "true"))
		TArray<UMaterialInstance*> MaterialsRing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Values, meta = (AllowPrivateAccess = "true"))
		double DistanceToMoveDown = -1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Values, meta = (AllowPrivateAccess = "true"))
		double TimeUntilDrop = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Values, meta = (AllowPrivateAccess = "true"))
		double TimeUntilReset = 8.f;

	// Component for all carrot audio
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkComponent* CarrotAudio;

	// Rumble events
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WEStartRumble;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WEStopRumble;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Niagara, meta = (AllowPrivateAccess = "true"))
		class UNiagaraComponent* NiagaraComponent = nullptr;

	UFUNCTION()
		void MoveDownwards();

	UFUNCTION()
		void MoveUpwards();

	UFUNCTION()
		void DestroyCarrot();

	FTimerHandle MovementDownTimer{}, MovementUpTimer{}, DestroyTimer{};

	FVector InitialLocation{};

	FVector InitialActorLocation{};

	FVector InitialComponentLocation{};

	float AccumulatedTime{};

	bool bHitCarrot = false;

	bool bMoveDown = false;

	bool bMoveUp = false;

	bool bReset = false;

	FVector ResetOffset { 0.0, 0.0, 3000.0 };


	bool bIsEating{};
};
