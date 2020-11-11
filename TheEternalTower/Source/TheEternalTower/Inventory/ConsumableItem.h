// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Item.h"
#include "Structs/AttributeManipulation.h"
#include "ConsumableItem.generated.h"

using namespace UP;

UENUM(BlueprintType)
enum class EConsumableType : uint8
{
	TYPE_NONE			UMETA(DisplayName = "None"),
	TYPE_JUNK			UMETA(DisplayName = "Junk"),
	TYPE_BUFF			UMETA(DisplayName = "Buff Potion"),
	TYPE_ENHANCE_CAT	UMETA(DisplayName = "Enhancement Catalyst"),
	TYPE_ENHANCE_TOOL	UMETA(DisplayName = "Enhancement Tool"),
	TYPE_INSTANT		UMETA(DisplayName = "Instant Potion"),
	TYPE_CRAFTING		UMETA(DisplayName = "Crafting Material"),
	TYPE_TREASURE		UMETA(DisplayName = "Valuable Treasure")
};


UCLASS()
class THEETERNALTOWER_API AConsumableItem : public AItem
{
	GENERATED_BODY()

// PROPERTIES
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Item Properties")
	EConsumableType ConsumableEffect;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Consumable Item Properties")
	bool bIsStackable;

	UPROPERTY(ReplicatedUsing = OnRep_CountChange, EditAnywhere, BlueprintReadWrite, Category = "Consumable Item Properties", meta = (EditCondition = "bIsStackable", ExposeOnSpawn = true, ClampMin = 0))
	int32 Count;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Item Properties", meta = (EditCondition = "bIsStackable", ClampMin = 0))
	int32 MaxCount;

	UPROPERTY(BlueprintReadWrite, Category = "Consumable Item Properties")
	bool bIsInHotbar;

// FUNCTIONS
public:

	AConsumableItem();

	UFUNCTION(BlueprintCallable, Category = "Price Management")
	FCurrency GetTotalPrice();

protected:

	UFUNCTION()
	void OnRep_CountChange();
};
