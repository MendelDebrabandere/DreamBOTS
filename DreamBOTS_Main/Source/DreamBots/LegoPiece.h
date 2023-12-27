// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "LegoPiece.generated.h"

class ALegoDrone;
class ALegoBridge;

UCLASS()
class DREAMBOTS_API ALegoPiece : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALegoPiece();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void MakeCorrespondingPieceVisible();

	ALegoBridge* GetLegoBridge() { return LegoBridge; };
	UStaticMeshComponent* GetLegoPieceMesh() { return LegoPieceMesh; };

	void SetIsAttached(bool attached) { bIsAttached = attached; };
	void SetLDrone(ALegoDrone* LDrone);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Scene, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* LegoPieceMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* PickUpLegoBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Lego, meta = (AllowPrivateAccess = "true"))
		class ALegoBridge* LegoBridge;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Lego, meta = (AllowPrivateAccess = "true"))
		class ALegoDrone* LegoDrone;

	bool bIsAttached = false;
};
