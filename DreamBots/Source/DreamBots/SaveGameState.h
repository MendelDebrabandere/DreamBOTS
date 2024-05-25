// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameState.generated.h"

/**
 *
 */
UCLASS()
class DREAMBOTS_API USaveGameState : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY()
		TArray<bool> FoundMovieTapes;
	USaveGameState();
protected:
private:
};
