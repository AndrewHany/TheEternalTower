// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Inventory/Item.h"
#include "Inventory/EquippableItem.h"
#include "Inventory/BuffConsumable.h"
#include "Structs/Currency.h"
#include "Inventory.generated.h"


UENUM(BlueprintType)
enum class ESortBy : uint8
{
	SORT_BY_RARITY			UMETA(DisplayName = "Rarity"),
	SORT_BY_PRICE			UMETA(DisplayName = "Price"),
	SORT_BY_TYPE			UMETA(DisplayName = "Item Type"),
	SORT_BY_LEVEL			UMETA(DisplayName = "Required Level")
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class THEETERNALTOWER_API UInventory : public UActorComponent
{
	GENERATED_BODY()
	
// PROPERTIES
public:
	
	/** Total number of slots that should exist in the inventory including locked ones*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Size")
	uint8 NumberOfSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Size")
	uint8 SlotsPerRow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Size")
	uint8 NumberOfLockedSlots;

	UPROPERTY(BlueprintReadWrite, Category = "Inventory Size")
	uint8 UsedSlotCount;

	UPROPERTY(ReplicatedUsing = OnRep_UpdateMoney, BlueprintReadWrite, Category = "Money")
	FCurrency Money;

	UPROPERTY(ReplicatedUsing = OnRep_UpdateInventory, BlueprintReadWrite, Category = "Inventory Array")
	TArray<AItem*> Inventory;

	UPROPERTY(BlueprintReadWrite)
	TMap<int32, float> ServerItemsOnCooldown;

// FUNCTIONS
public:	
	UInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory Management")
	void InitializeInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory Management")
	void AddInventorySlot(int32 NumberOfNewSlots);

	UFUNCTION(BlueprintCallable, Category = "Inventory Management")
	void RemoveInventorySlot();

	/** 
	Adds a new item to the character's inventory and stacks a consumable item if it already exists.
	Pass a number that indicates which slot you want the item to be added in as the AddIndex param,
	or leave it as the default '-1' to place it in the first empty slot.
	
	Returns true if item is added successfully.*/
	UFUNCTION(BlueprintCallable, Category = "Inventory Management")
	bool AddItemToInventory(AItem* NewItem, int32 AddIndex = -1);

	UFUNCTION(BlueprintCallable, Category = "Inventory Management")
	void RemoveItemFromInventory(AItem* Item, bool bShouldDestroy = false);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Inventory Management")
	void ServerSwapItems				(AItem* FirstItem, int32 FirstIndex, AItem* SecondItem, int32 SecondIndex);
	void ServerSwapItems_Implementation (AItem* FirstItem, int32 FirstIndex, AItem* SecondItem, int32 SecondIndex);
	bool ServerSwapItems_Validate		(AItem* FirstItem, int32 FirstIndex, AItem* SecondItem, int32 SecondIndex);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Inventory Management")
	void ServerSplitStackableItem				 (AItem* ItemToSplit, int32 NewStackCount, int32 SplitFromIndex, int32 SplitToIndex);
	void ServerSplitStackableItem_Implementation (AItem* ItemToSplit, int32 NewStackCount, int32 SplitFromIndex, int32 SplitToIndex);
	bool ServerSplitStackableItem_Validate		 (AItem* ItemToSplit, int32 NewStackCount, int32 SplitFromIndex, int32 SplitToIndex);


	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Inventory Management")
	void ServerSortInventory				(ESortBy SortInventoryBy);
	void ServerSortInventory_Implementation	(ESortBy SortInventoryBy);
	bool ServerSortInventory_Validate		(ESortBy SortInventoryBy);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Inventory Management")
	void ServerAutoStack();
	void ServerAutoStack_Implementation();
	bool ServerAutoStack_Validate();


	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Inventory Management")
	void ServerUnlockSlots					(uint8 NumberOfSlotsToUnlock);
	void ServerUnlockSlots_Implementation	(uint8 NumberOfSlotsToUnlock);
	bool ServerUnlockSlots_Validate			(uint8 NumberOfSlotsToUnlock);


	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Inventory Management")
	void ClientUnlockSlots					(uint8 NumberOfSlotsToUnlock);
	void ClientUnlockSlots_Implementation	(uint8 NumberOfSlotsToUnlock);

	UFUNCTION()
	void DeactivateItem(int32 ItemID, float Cooldown);

	UFUNCTION()
	void ReactivateItem(int32 ItemID, float Cooldown);

protected:
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float Deltatime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool AddStackableConsumableItem(AConsumableItem* NewConsumableItem, int32 AddIndex = -1);

	UFUNCTION()
	void OnRep_UpdateInventory();

	UFUNCTION()
	void OnRep_UpdateMoney();
};
