// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Inventory/Item.h"
#include "Structs/AttributeManipulation.h"
#include "EquippableItem.generated.h"

using namespace UP;

UENUM(BlueprintType)
enum class EEquipmentType : uint8 
{
	EQUIPMENT_WEAPON	UMETA(DisplayName = "Weapon"),
	EQUIPMENT_HELMET	UMETA(DisplayName = "Helmet"),
	EQUIPMENT_ARMOR		UMETA(DisplayName = "Armor"),
	EQUIPMENT_GLOVES	UMETA(DisplayName = "Gloves"),
	EQUIPMENT_BOOTS		UMETA(DisplayName = "Boots"),
	EQUIPMENT_RING		UMETA(DisplayName = "Ring"),
	EQUIPMENT_NECK		UMETA(DisplayName = "Necklace"),
	EQUIPMENT_EARRING	UMETA(DisplayName = "Earring")
};

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	CLASS_ANY		UMETA(DisplayName = "Any Class"),
	CLASS_WARRIOR	UMETA(DisplayName = "Warrior"),
	CLASS_WIZARD	UMETA(DisplayName = "Wizard"),
	CLASS_PRIEST	UMETA(DisplayName = "Priest")
};


UCLASS()
class THEETERNALTOWER_API AEquippableItem : public AItem
{
	GENERATED_BODY()
	
// PROPERTIES
public:

	/** Only used for a weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Properties")
	EEquipmentType EquipmentType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Properties")
	bool bClassSpecific;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Properties", meta = (EditCondition = "bClassSpecific"))
	ECharacterClass EquipmentClass;

	/** Only used for an armor set.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Properties")
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Properties")
	bool bCanBeEnhanced;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Properties", meta = (EditCondition = "bCanBeEnhanced"))
	uint8 CurrentEnhancementLevel;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Properties|Main Attributes")
	TArray<FAttributeManipulation> BaseAttributes;

	/** 
	Total bonus stats that should be applied when the
	item has reached its maximum enhancement level.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Properties|Bonus Attributes", meta = (EditCondition = "!bEnhanceBaseAttributes"))
	TArray<FAttributeManipulation> EnhancementAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Properties|Bonus Attributes", meta = (EditCondition = "bCanBeEnhanced"))
	bool bEnhanceBaseAttributes;


// FUNCTIONS
public:

	AEquippableItem();

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	TArray<FAttributeManipulation> CalculateBonusStatsBasedOnEnhancementLevel();

	virtual void ServerUseItem_Implementation() override;
};
