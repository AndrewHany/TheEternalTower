// Fill out your copyright notice in the Description page of Project Settings.

#include "TheEternalTower.h"
#include "Party.h"


// Sets default values
AParty::AParty()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SetActorHiddenInGame(true);
	AParty::Size = 0;
	AParty::MaxSize = 5;
	bReplicates = true;
	bAlwaysRelevant = true;

}

void AParty::Initialize(ATheEternalTowerCharacter* Lead, uint8 PartyID)
{
	AParty::ID = PartyID;
	AParty::Leader = Lead;
	AParty::AddPlayer(Lead);
}

// Called when the game starts or when spawned
void AParty::BeginPlay()
{
	Super::BeginPlay();

}

void AParty::AddPlayer(ATheEternalTowerCharacter* Character)
{
	if (Size < MaxSize)
	{
		Players.Add(Character);
		Size++;
	}
}

void AParty::RemovePlayer(ATheEternalTowerCharacter* Character)
{
	if (Size > 1)
	{
		Players.Remove(Character);
		Size--;
	}
	else
	{
		AParty::Destroy();
	}
}

bool AParty::Full()
{
	return Size == MaxSize;
}

void AParty::Destroy()
{
	Players.~TArray();
}




