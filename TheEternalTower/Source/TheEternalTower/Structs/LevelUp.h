// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "LevelUp.generated.h"

/** Structure that defines a level up table entry */
USTRUCT(BlueprintType)
struct FLevelUp : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LevelUp)
	int32 ExpToNextLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LevelUp)
	float AddedHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LevelUp)
	float AddedMana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LevelUp)
	float AddedPhysicalAttack;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LevelUp)
	float AddedMagicalAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LevelUp)
	float AddedPhysicalDefense;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LevelUp)
	float AddedMagicalDefense;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LevelUp)
	float AddedAccuracy;


	FLevelUp()
	{
		ExpToNextLevel = 0;
		AddedHealth = 0.0f;
		AddedMana = 0.0f;
		AddedPhysicalAttack = 0.0f;
		AddedMagicalAttack = 0.0f;
		AddedPhysicalDefense = 0.0f;
		AddedMagicalDefense = 0.0f;
		AddedAccuracy = 0.0f;
	};
};