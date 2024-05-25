// Fill out your copyright notice in the Description page of Project Settings.

#include "DreamBotsGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGameState.h"

UDreamBotsGameInstance::UDreamBotsGameInstance()
{
	MovieTapesArray.Init(false, 16);
//	CurrentMovieTapeIdx = 0;
}

void UDreamBotsGameInstance::Init()
{
	USaveGameState* saveState = Cast<USaveGameState>(UGameplayStatics::GetGameInstance(GetWorld()));
	saveState = Cast<USaveGameState>(UGameplayStatics::LoadGameFromSlot("Slot", 0));
	if (saveState)
	{
		MovieTapesArray = saveState->FoundMovieTapes;
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Loaded!"));

	}
}

void UDreamBotsGameInstance::Shutdown()
{
	USaveGameState* saveState = Cast<USaveGameState>(UGameplayStatics::CreateSaveGameObject(USaveGameState::StaticClass()));
	if (saveState)
	{
		saveState->FoundMovieTapes = MovieTapesArray;
		UGameplayStatics::SaveGameToSlot(saveState, "Slot", 0);
	}
}
