// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovieTape.h"
#include "CleaningWall.h"
#include "PoisonFloor.generated.h"



UCLASS()
class DREAMBOTS_API APoisonFloor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APoisonFloor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	class UTextureRenderTarget2D* PoisonRenderTarget;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Custom")
	void DoCharacterLineTrace();

	UFUNCTION(BlueprintImplementableEvent, Category = "Custom")
	void ClearFloor();


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* MovieTapeSpawnSpot{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* RaycastFloor{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* OverlapBox{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* PoisonDecal{};



	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool bCharacterInZone{};

	UPROPERTY(EditAnywhere, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	class UMaterial* DrawingMaterial{};

	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicDrawingMaterial{};

	UFUNCTION(BlueprintCallable)
	void DrawToRT(const FVector2D& pos);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class ADreamBotsCharacter* Character{};

	UPROPERTY(EditAnywhere, Category = "Variables")
	TSubclassOf<class AMovieTape> MovieTapeClass{};

	UPROPERTY(EditAnywhere, Category = "Drawing")
		TEnumAsByte<DrawingType> MovieTapeEnum = GeldDrone;

	UPROPERTY(EditAnywhere, Category = "Variables")
	UTextureRenderTarget2D* MyRenderTarget;



	float LineTracesPerSecond{ 20.f };
	float Timer{};

	void CheckRenderTargetBlackPercentage();
	void ReadPixelsThreaded();

	FTimerHandle TimerHandle;

	FTextureRenderTargetResource* PoisonRenderTargetResource{};
	int PercentageFilled{};
	bool bCleaning{ false };
	bool bCleaned{ false };

	UPROPERTY(EditAnywhere, Category = Walls)
	TArray<ACleaningWall*> Walls{};


	UPROPERTY(EditAnywhere, Category = Walls)
	int DestroyPercentage{ 35 };

	UPROPERTY(EditAnywhere, Category = Walls)
	int CleanPercentage{ 8 };
};
