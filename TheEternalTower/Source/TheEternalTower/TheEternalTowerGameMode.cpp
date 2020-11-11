// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "TheEternalTower.h"
#include "TheEternalTowerGameMode.h"
#include "Engine.h"
#include "UnrealString.h"
#include "GameFramework/Actor.h"
#include "TheEternalTowerCharacter.h"

ATheEternalTowerGameMode::ATheEternalTowerGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_TheEternalTowerCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

}