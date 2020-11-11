// Fill out your copyright notice in the Description page of Project Settings.

#include "TheEternalTower.h"
#include "Structs/EnhancementData.h"
#include "TheEternalTowerCharacter.h"
#include "EquippableItem.h"
#include "Engine.h"


AEquippableItem::AEquippableItem()
{
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
}


TArray<FAttributeManipulation> AEquippableItem::CalculateBonusStatsBasedOnEnhancementLevel()
{
	TArray<FAttributeManipulation> BonusStats;

	FString DataPath = "DataTable'/Game/DataTables/EquipmentEnhancementData.EquipmentEnhancementData'";
	UDataTable* EquipmentEnhancementDataTable = ConstructorHelpersInternal::FindOrLoadObject<UDataTable>(DataPath);

	FEnhancementData* Row;
	Row = EquipmentEnhancementDataTable->FindRow<FEnhancementData>(FName(*FString::FromInt((int32)CurrentEnhancementLevel)), TEXT(""));

	if (Row)
	{

		float BonusStatsPercentage = Row->BonusStatsPercentage / 100.0f;

		if (bEnhanceBaseAttributes)
		{
			BonusStats = BaseAttributes;
		}
		else
		{
			BonusStats = EnhancementAttributes;
		}

		for (auto& Attribute : BonusStats)
		{
			Attribute.Value *= BonusStatsPercentage;
		}
	}

	return BonusStats;
}


void AEquippableItem::ServerUseItem_Implementation()
{
	ATheEternalTowerCharacter* MyCharacter = Cast<ATheEternalTowerCharacter>(GetOwner());
	
	if (MyCharacter)
	{
		MyCharacter->ServerEquipItem(this);
	}
}
