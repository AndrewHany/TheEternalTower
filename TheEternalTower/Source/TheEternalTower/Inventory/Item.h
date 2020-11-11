// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Structs/Currency.h"
#include "Item.generated.h"

UENUM(BlueprintType)		
enum class EItemRarity : uint8
{
	RARITY_COMMON		UMETA(DisplayName = "Common"),
	RARITY_RARE			UMETA(DisplayName = "Rare"),
	RARITY_EPIC			UMETA(DisplayName = "Epic"),
	RARITY_LEGENDARY	UMETA(DisplayName = "Legendary"),
	RARITY_GODLIKE		UMETA(DisplayName = "Godlike")
};

#define COMMON_COLOR	 FLinearColor(1.0f, 1.0f, 1.0f)			//white
#define RARE_COLOR		 FLinearColor(0.074f, 0.07f, 0.644f)	//blue
#define EPIC_COLOR		 FLinearColor(1.0f, 0.15f, 1.0f)		//purple
#define LEGENDARY_COLOR	 FLinearColor(0.947f, 0.708f, 0.018f)	//orange/gold
#define GODLIKE_COLOR	 FLinearColor(0.761f, 0.001f, 0.001f)	//red


UENUM(BlueprintType)
enum class EItemType : uint8
{
	TYPE_ARMOR			UMETA(DisplayName = "Weapon"),
	TYPE_WEAPON			UMETA(DisplayName = "Armor"),
	TYPE_ACCESSORY		UMETA(DisplayName = "Accessory"),
	TYPE_CONSUMABLE		UMETA(DisplayName = "Consumable"),
	TYPE_MISC			UMETA(DisplayName = "Miscellaneous")
};


UCLASS()
class THEETERNALTOWER_API AItem : public AActor
{
	GENERATED_BODY()

// PROPERTIES
public:
	
	/** Every item should be given a unique identifier.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	int32 ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	EItemType ItemType;

	/** Refers to the name that will be shown to any user*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	FString ItemName;

	/** Point this to the 2D texture that will be shown as an icon for the item*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	EItemRarity ItemRarity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	bool bCanBeDropped;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	bool bCanBeSold;

	/** The required level to be able to use this item*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	int32 RequiredLevel;

	/** Determines how much the user would get if the item is sold at a merchant*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (EditCondition = "bCanBeSold"))
	FCurrency SalePrice;

	/** A fancy description of the item*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (MultiLine))
	FString FlavorText;

	/** A fancy description of the item*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	USoundBase* HandleSoundEffect;

	/** Generated automatically based on the item's selected behavior*/
	UPROPERTY(BlueprintReadWrite, Category = "Item Properties", meta = (MultiLine))
	FString EffectText;

	/** The ID of the player that currently holds this item, -1 if it has no owner*/
	UPROPERTY(BlueprintReadWrite, Category = "Item Properties")
	int32 OwningPlayerID;

	/** Index of the inventory slot that this item is currently in*/
	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Item Properties")
	int32 CurrentSlotIndex;

// FUNCTIONS
public:	

	AItem();

	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	FLinearColor GetColorBasedOnRarity();

	/** 
	[Server] An overridable function that specifies the effect of using this item.
	This function automatically determines the item's effect based on the selected
	options but can be overridden using blueprints.
	*/
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	virtual void ServerUseItem();
	virtual void ServerUseItem_Implementation();
	virtual bool ServerUseItem_Validate();

	UFUNCTION(BlueprintCallable, Client, Unreliable)
	void ClientItemMoved();
	void ClientItemMoved_Implementation();

protected:
	 
	virtual void BeginPlay() override;
};
