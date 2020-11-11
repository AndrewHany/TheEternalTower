// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "AttributeManipulation.generated.h"

UENUM(BlueprintType)
enum class EAttribute : uint8
{
	ATTRIB_HEALTH		UMETA(DisplayName = "Health"),
	ATTRIB_MAX_HEALTH	UMETA(DisplayName = "Maximum Health"),
	ATTRIB_MANA			UMETA(DisplayName = "Mana"),
	ATTRIB_MAX_MANA		UMETA(DisplayName = "Maximum Mana"),
	ATTRIB_ACC			UMETA(DisplayName = "Accuracy"),
	ATTRIB_MIN_PHYATK	UMETA(DisplayName = "Minimum Physical Attack"),
	ATTRIB_MAX_PHYATK	UMETA(DisplayName = "Maximum Physical Attack"),
	ATTRIB_MIN_MAGATK	UMETA(DisplayName = "Minimum Magical Attack"),
	ATTRIB_MAX_MAGATK	UMETA(DisplayName = "Maximum Magical Attack"),
	ATTRIB_PHYDEF		UMETA(DisplayName = "Physical Defense"),
	ATTRIB_MAGDEF		UMETA(DisplayName = "Magical Defense"),
	ATTRIB_EVASION		UMETA(DisplayName = "Evasion"),
	ATTRIB_DMGREDUCE	UMETA(DisplayName = "Damage Reduction"),
	ATTRIB_LIFESTEAL	UMETA(DisplayName = "Life Steal"),
	ATTRIB_CRITRATE		UMETA(DisplayName = "Critical Rate"),
	ATTRIB_CRITDMG		UMETA(DisplayName = "Critical Damage"),
	ATTRIB_LUCK 		UMETA(DisplayName = "Luck"),
	ATTRIB_MOVESPD		UMETA(DisplayName = "Movement Speed"),
	ATTRIB_EXP			UMETA(DisplayName = "Experience")
};



USTRUCT(BlueprintType)
struct FAttributeManipulation
{
	GENERATED_USTRUCT_BODY()

public:

	FAttributeManipulation() {};

	/** Specifies the attribute that the change would be applied to*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttribute AffectedAttribute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsPercentile;

	/**
	The value of the change that will happen to the attribute.
	
	Note: If Is Percentile is checked, then this value will indicate a percentage.
	
	For Example:
	Maximum Health    , 120 -> Would result in a 120% increase of the Health attribute.
	Physical Defense  , -50 -> Would result in a  50% decrease of the Physical Defense attribute.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value;
};