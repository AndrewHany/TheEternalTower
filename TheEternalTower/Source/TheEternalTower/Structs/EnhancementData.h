// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "EnhancementData.generated.h"

/** Structure that defines enhancement data entry for an equippable item */
USTRUCT(BlueprintType)
struct FEnhancementData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	/**
	The probability of successfully
	upgrading an item to this level.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LevelUp)
	float ChanceOfSuccess;

	/** 
	Determines how many enhancement levels the 
	item loses if it fails to reach this level.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LevelUp)
	uint8 UpgradesLostUponFailure;

	/**
	How much should the item have of the reachable
	bonus stats upon achieving this enhancement level.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LevelUp)
	float BonusStatsPercentage;

	FEnhancementData()
	{
		ChanceOfSuccess = 0.0f;
		UpgradesLostUponFailure = 0;
		BonusStatsPercentage = 0.0f;
	};
};