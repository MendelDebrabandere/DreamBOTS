// Copyright Epic Games, Inc. All Rights Reserved.

#include "DreamBotsGameMode.h"
#include "DreamBotsCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADreamBotsGameMode::ADreamBotsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
