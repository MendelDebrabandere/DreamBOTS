// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MovieTape.h"
#include "DreamBotsGameInstance.generated.h"

/**
 *
 */
UCLASS()
class DREAMBOTS_API UDreamBotsGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<bool> MovieTapesArray;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//	int CurrentMovieTapeIdx;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<DrawingType> CurrentMovieTape = GeldDrone;

	UFUNCTION(BlueprintCallable)
		bool HasPlayerEnteredStartscreenBefore() const { return bHasEnteredStartScreenBefore; }
	UFUNCTION(BlueprintCallable)
		void SetHasPlayerEnteredStartScreenBefore(bool flag) { bHasEnteredStartScreenBefore = flag; }

	UFUNCTION(BlueprintCallable)
		bool IsUsingGamePad() const { return bIsUsingGamepad; }
	UFUNCTION(BlueprintCallable)
		void SetUsingGamepad(bool flag) { bIsUsingGamepad = flag; }


	UDreamBotsGameInstance();
protected:
	virtual void Init();
	virtual void Shutdown();
private:
	bool bHasEnteredStartScreenBefore = false;
	bool bIsUsingGamepad = false;
};
