// Fill out your copyright notice in the Description page of Project Settings.

#include "TheEternalTower.h"
#include "Inventory.h"
#include "TheEternalTowerPlayerController.h"
#include "UnrealNetwork.h"
#include "Engine.h"


// Sets default values
UInventory::UInventory()
{
	PrimaryComponentTick.bCanEverTick = true;

	bReplicates = true;

	SlotsPerRow = 5;
	NumberOfSlots = 25;
	UsedSlotCount = 0;
	NumberOfLockedSlots = 0;
	Money = FCurrency(1, 50, 50);
}

// Replication
void UInventory::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UInventory, Inventory, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UInventory, Money, COND_OwnerOnly);
}

// Called when the game starts or when spawned
void UInventory::BeginPlay()
{
	Super::BeginPlay();
	//InitializeInventory();
}


void UInventory::InitializeInventory()
{
	if (GetOwner()->Role == ROLE_Authority)
	{
		for (int32 Count = 0; Count < NumberOfSlots - NumberOfLockedSlots ; Count++)
			Inventory.Add(NULL);
	}
}


void UInventory::AddInventorySlot(int32 NumberOfNewSlots)
{
	if (GetOwner()->Role == ROLE_Authority)
	{
		for (int32 Count = 0; Count < NumberOfNewSlots; Count++)
		{
			Inventory.Add(NULL);
			NumberOfSlots++;
		}
	}
}


void UInventory::RemoveInventorySlot()
{
	if (!Inventory.Last())
	{
		NumberOfSlots--;
		Inventory.Pop();
	}		
}


bool UInventory::AddItemToInventory(AItem* NewItem, int32 AddIndex)
{
	if (GetOwner()->Role != ROLE_Authority || UsedSlotCount == NumberOfSlots)
		return false;

	// Try casting our item to see if it's a consumable
	AConsumableItem* NewConsumableItem = Cast<AConsumableItem>(NewItem);
	if (NewConsumableItem)
	{
		// If the consumable item can be stacked
		if (NewConsumableItem->bIsStackable)
		{	
			// Call a function that adds stackable items
			if (AddStackableConsumableItem(NewConsumableItem, AddIndex))
			{
				float* Cooldown = ServerItemsOnCooldown.Find(NewItem->ItemID);
				
				if (Cooldown)
				{
					if (*Cooldown > 0.0f)
					{
						ABuffConsumable* BuffItem = Cast<ABuffConsumable>(NewItem);

						if (BuffItem)
						{
							BuffItem->ServerRemainingCooldown = *Cooldown;
							BuffItem->bOnCooldown = true;
						}
					}
				}
				
				return true;
			}
		}
	}
	// If it's not stackable or not a consumable at all, then just add it
	if (AddIndex == -1)
	{
		// Go through the inventory to find the first empty slot
		for (int32 Index = 0; Index != Inventory.Num(); Index++)
		{
			AItem* CurrentItem = Inventory[Index];

			if (!CurrentItem)
			{
				// Item is null so slot is empty, add item and return
				Inventory[Index] = NewItem;
				NewItem->CurrentSlotIndex = Index;
				NewItem->SetOwner(GetOwner());
				NewItem->ClientItemMoved();
				
				UsedSlotCount++;
				return true;
			}
		}
		// If we go through the loop without finding 
		// an empty slot then inventory's full (extra check)
		return false;
	}
	else
	{
		// If provided an index, check if that slot is empty
		if (!Inventory[AddIndex])
		{
			// If so, then add the item to it
			Inventory[AddIndex] = NewItem;
			NewItem->CurrentSlotIndex = AddIndex;
			NewItem->SetOwner(GetOwner());
			NewItem->ClientItemMoved();

			UsedSlotCount++;
			return true;
		}
		// if not, then item cannot be added to that slot
		return false;
	}
}


bool UInventory::AddStackableConsumableItem(AConsumableItem* NewConsumableItem, int32 AddIndex)
{
	if (GetOwner()->Role != ROLE_Authority || UsedSlotCount == NumberOfSlots)
		return false;

	// Check if there exists an item already with the same item id
	// and that it can accept more items into its stack
	auto* ExistingConsumableItem = Inventory.FindByPredicate([&](AItem* Item) {
		if (Item)
		{
			AConsumableItem* CurrentConsumableItem = Cast<AConsumableItem>(Item);

			if (CurrentConsumableItem)
			{
				return (CurrentConsumableItem->Count < CurrentConsumableItem->MaxCount)
					&& (CurrentConsumableItem->ItemID == NewConsumableItem->ItemID);
			}
		}
		
		return false;
	});

	// If there is such an item
	if (ExistingConsumableItem)
	{
		AConsumableItem* CurrentConsumableItem = Cast<AConsumableItem>((*ExistingConsumableItem));

		// Check if the sum of both our stacks is less than the maximum count
		if (CurrentConsumableItem->Count + NewConsumableItem->Count <= CurrentConsumableItem->MaxCount)
		{
			// Then just add the new stack to our existing stack and destroy the new item
			CurrentConsumableItem->Count += NewConsumableItem->Count;
			CurrentConsumableItem->ClientItemMoved();
			NewConsumableItem->Destroy();

			return true;
		}
		else
		{
			// If not, then check how much is left over after reaching the maximum allowable count
			int32 LeftOverCount = CurrentConsumableItem->Count + NewConsumableItem->Count - CurrentConsumableItem->MaxCount;
			
			CurrentConsumableItem->Count = CurrentConsumableItem->MaxCount;

			// Set the new stack to the left over items and recall function
			NewConsumableItem->Count = LeftOverCount;

			if (AddStackableConsumableItem(NewConsumableItem))
				return true;
			else
				return false;
		}
	}
	else
	{
		// If there is no existing item that meets our conditions, then just add a new item
		
		bool bIsItemAdded = false;
		
		if (AddIndex == -1)
		{
			// Go through the inventory to find the first empty slot
			for (int32 Index = 0; Index != Inventory.Num(); Index++)
			{
				AItem* CurrentItem = Inventory[Index];

				if (!CurrentItem)
				{
					// Item is null so we found a slot that 
					// is empty, then add item and break
					Inventory[Index] = NewConsumableItem;
					NewConsumableItem->CurrentSlotIndex = Index;
					NewConsumableItem->SetOwner(GetOwner());
					UsedSlotCount++;
					
					bIsItemAdded = true;
					break;
				}
			}
		}
		else
		{
			// If provided an index, check if that slot is empty
			if (!Inventory[AddIndex])
			{
				// If so, then add the item to it
				Inventory[AddIndex] = NewConsumableItem;
				NewConsumableItem->CurrentSlotIndex = AddIndex;
				NewConsumableItem->SetOwner(GetOwner());
				UsedSlotCount++;
				
				bIsItemAdded = true;
			}
		}
		
		// If this point is reached without adding an item
		// then it cannot be added so we return false
		if (bIsItemAdded == false)
		{
			return false;
		}
		
		// We need to check if our new stack already exceeds the allowable limit
		if (NewConsumableItem->Count > NewConsumableItem->MaxCount)
		{
			int32 LeftOverCount = NewConsumableItem->Count - NewConsumableItem->MaxCount;
			NewConsumableItem->Count = NewConsumableItem->MaxCount;

			// Clone the parameters from this item
			FActorSpawnParameters Parameters;
			Parameters.Template = NewConsumableItem;
			
			// Then spawn a new item with those parameters
			AConsumableItem* DuplicatedConsumableItem = GetWorld()->SpawnActor<class AConsumableItem>(NewConsumableItem->GetClass(), Parameters);
			
			DuplicatedConsumableItem->Count = LeftOverCount;

			//recall function with the new copied item but with the leftover count
			if (AddStackableConsumableItem(DuplicatedConsumableItem))
				return true;
			else
				return false;
		}

		// If this point is reached, then item was added successfully
		NewConsumableItem->ClientItemMoved();
		return true;
	}
}


void UInventory::RemoveItemFromInventory(AItem* Item, bool bShouldDestroy)
{
	int32 Index;
	if (Inventory.Find(Item, Index))
	{
		Inventory[Index] = NULL;
		Item->CurrentSlotIndex = -1;
		UsedSlotCount--;
		
		if (bShouldDestroy)
		{
			Item->Destroy();
		}
	}
}


void UInventory::ServerSwapItems_Implementation(AItem* FirstItem, int32 FirstIndex, AItem* SecondItem, int32 SecondIndex)
{	
	
	if (FirstItem && FirstItem == Inventory[FirstIndex] && SecondItem && SecondItem == Inventory[SecondIndex])
	{
		if (FirstItem->ItemID == SecondItem->ItemID)
		{
			AConsumableItem* FirstConsumableItem = Cast<AConsumableItem>(FirstItem);
			AConsumableItem* SecondConsumableItem = Cast<AConsumableItem>(SecondItem);

			if (FirstConsumableItem && SecondConsumableItem)
			{
				if (SecondConsumableItem->Count < SecondConsumableItem->MaxCount)
				{
					if (FirstConsumableItem->Count + SecondConsumableItem->Count <= FirstConsumableItem->MaxCount)
					{
						SecondConsumableItem->Count += FirstConsumableItem->Count;
						SecondConsumableItem->ClientItemMoved();
						
						ABuffConsumable* FirstBuffConsumable = Cast <ABuffConsumable>(FirstConsumableItem);

						if (FirstBuffConsumable)
						{
							if (FirstBuffConsumable->bIsActive)
							{
								RemoveItemFromInventory(FirstConsumableItem, false);
								return;
							}
						}

						RemoveItemFromInventory(FirstConsumableItem, true);

						return;
					}
					else
					{
						int32 LeftOverCount = FirstConsumableItem->Count + SecondConsumableItem->Count - FirstConsumableItem->MaxCount;

						SecondConsumableItem->Count = SecondConsumableItem->MaxCount;
						FirstConsumableItem->Count = LeftOverCount;

						FirstConsumableItem->ClientItemMoved();

						return;
					}
				}
			}
		}

		Inventory.Swap(FirstIndex, SecondIndex);

		FirstItem->CurrentSlotIndex = SecondIndex;
		SecondItem->CurrentSlotIndex = FirstIndex;

	}
	else if (FirstItem == Inventory[FirstIndex] && !Inventory[SecondIndex])
	{
		Inventory[SecondIndex] = FirstItem;
		Inventory[FirstIndex] = NULL;

		FirstItem->CurrentSlotIndex = SecondIndex;
	}

	FirstItem->ClientItemMoved();

}


bool UInventory::ServerSwapItems_Validate(AItem* FirstItem, int32 FirstIndex, AItem* SecondItem, int32 SecondIndex)
{
	if (Inventory.Find(FirstItem) || Inventory.Find(SecondItem))
	{
		return true;
	}
	return false;
}



void UInventory::ServerSplitStackableItem_Implementation(AItem* ItemToSplit, int32 NewStackCount, int32 SplitFromIndex, int32 SplitToIndex)
{
	if (!Inventory[SplitToIndex] && ItemToSplit && Inventory[SplitFromIndex] == ItemToSplit)
	{
		AConsumableItem* ConsumableToSplit = Cast<AConsumableItem>(ItemToSplit);
		
		if (ConsumableToSplit)
		{
			if (ConsumableToSplit->bIsStackable && ConsumableToSplit->Count > NewStackCount)
			{
				// Clone the parameters from this item
				FActorSpawnParameters Parameters;
				Parameters.Template = ConsumableToSplit;

				// Then spawn a new item with those parameters
				AConsumableItem* NewConsumableItem = GetWorld()->SpawnActor<AConsumableItem>(ConsumableToSplit->GetClass(), Parameters);
				
				ABuffConsumable* NewBuffConsumable = Cast<ABuffConsumable>(NewConsumableItem);
				
				if (NewBuffConsumable)
				{
					NewBuffConsumable->bIsActive = false;
					NewBuffConsumable->ClientRemainingTime = 0.0f;
					NewBuffConsumable->ServerRemainingTime = 0.0f;
					NewBuffConsumable->BuffTimerHandle.Invalidate();
				}

				NewConsumableItem->Count  = NewStackCount;
				ConsumableToSplit->Count -= NewStackCount;

				NewConsumableItem->CurrentSlotIndex = SplitToIndex;
				ConsumableToSplit->ClientItemMoved();
				UsedSlotCount++;

				NewConsumableItem->SetOwner(GetOwner());

				Inventory[SplitToIndex] = NewConsumableItem;
			}
		}
	}
}


bool UInventory::ServerSplitStackableItem_Validate(AItem* ItemToSplit, int32 NewStackCount, int32 SplitFromIndex, int32 SplitToIndex)
{
	if(ItemToSplit)
		return true;
	
	return false;
}


void UInventory::ServerSortInventory_Implementation(ESortBy SortInventoryBy)
{
	TArray<AItem*> ItemsToSort;

	for (auto& CurrentItem : Inventory)
	{
		if (CurrentItem)
			ItemsToSort.Add(CurrentItem);
	}

	ItemsToSort.StableSort([&](AItem& FirstItem, AItem& SecondItem) {

		AConsumableItem* FirstConsumableItem  = Cast<AConsumableItem>(&FirstItem);
		AConsumableItem* SecondConsumableItem = Cast<AConsumableItem>(&SecondItem);

		if (FirstConsumableItem && SecondConsumableItem)
		{
			if (FirstConsumableItem->ItemID == SecondConsumableItem->ItemID)
			{
				if (FirstConsumableItem->bIsStackable)
				{
					return FirstConsumableItem->Count >= SecondConsumableItem->Count;
				}
			}
		}
		
		switch (SortInventoryBy)
		{
		case ESortBy::SORT_BY_LEVEL:
		{
			if (FirstItem.RequiredLevel == SecondItem.RequiredLevel)
			{
				return FirstItem.ItemRarity >= SecondItem.ItemRarity;
			}
			else
			{
				return FirstItem.RequiredLevel >= SecondItem.RequiredLevel;
			}
		}
		case ESortBy::SORT_BY_RARITY:
		{
			if (FirstItem.ItemRarity == SecondItem.ItemRarity)
			{
				return FirstItem.RequiredLevel >= SecondItem.RequiredLevel;
			}
			else
			{
				return FirstItem.ItemRarity >= SecondItem.ItemRarity;
			}
		}
		case ESortBy::SORT_BY_PRICE:
		{
			if (FirstConsumableItem && !SecondConsumableItem)
			{
				return FirstConsumableItem->GetTotalPrice() >= SecondItem.SalePrice;
			}
			else if (!FirstConsumableItem && SecondConsumableItem)
			{
				return FirstItem.SalePrice >= SecondConsumableItem->GetTotalPrice();
			}
			else if (FirstConsumableItem && SecondConsumableItem)
			{
				return FirstConsumableItem->GetTotalPrice() >= SecondConsumableItem->GetTotalPrice();
			}
			else
			{
				return FirstItem.SalePrice >= SecondItem.SalePrice;
			}
		}
		case ESortBy::SORT_BY_TYPE:
		{
			if (FirstItem.ItemType == SecondItem.ItemType)
			{
				//armor or weapon
				if (FirstItem.ItemType <= EItemType::TYPE_WEAPON)
				{
					AEquippableItem* FirstEquipment = Cast<AEquippableItem>(&FirstItem);
					AEquippableItem* SecondEquipment = Cast<AEquippableItem>(&SecondItem);

					if (FirstEquipment && SecondEquipment)
					{
						if (FirstEquipment->EquipmentType == SecondEquipment->EquipmentType)
						{
							return FirstEquipment->CurrentEnhancementLevel >= SecondEquipment->CurrentEnhancementLevel;
						}
						else
						{
							return FirstEquipment->EquipmentType <= SecondEquipment->EquipmentType;
						}
					}
				}
				else if (FirstItem.ItemType == EItemType::TYPE_ACCESSORY)
				{
					return FirstItem.RequiredLevel >= SecondItem.RequiredLevel;
				}
				else if (FirstItem.ItemType == EItemType::TYPE_CONSUMABLE)
				{
					return FirstConsumableItem->ConsumableEffect <= SecondConsumableItem->ConsumableEffect;
				}
				else
				{
					return FirstItem.ItemRarity >= SecondItem.ItemRarity;
				}
			}
			else
			{
				return FirstItem.ItemType <= SecondItem.ItemType;
			}
		}
		default:
			return FirstItem.ItemID < SecondItem.ItemID;
		}
	});
	

	for (int32 Index = 0; Index < ItemsToSort.Num(); Index++)
	{
		Inventory[Index] = ItemsToSort[Index];
		Inventory[Index]->CurrentSlotIndex = Index;
	}

	for (int32 Index = ItemsToSort.Num(); Index != Inventory.Num(); Index++)
	{
		Inventory[Index] = NULL;
	}
}


bool UInventory::ServerSortInventory_Validate(ESortBy SortInventoryBy)
{
	return true;
}


void UInventory::ServerAutoStack_Implementation()
{
	TArray<AItem*> ItemsToStack;
	TMap<int32, int32> ItemsCount;

	for (int32 Index = 0; Index < Inventory.Num(); Index++)
	{
		AItem* CurrentItem = Inventory[Index];
		ItemsToStack.Add(CurrentItem);

		if (CurrentItem)
		{
			AConsumableItem* ConsumableItem = Cast<AConsumableItem>(CurrentItem);

			if (ConsumableItem)
			{
				int32* CurrentCount = ItemsCount.Find(ConsumableItem->ItemID);

				if (CurrentCount)
				{
					*CurrentCount += ConsumableItem->Count;
				}
				else
				{
					ItemsCount.Add(ConsumableItem->ItemID, ConsumableItem->Count);
				}
			}
		}
	}

	for (int32 Index = 0; Index < ItemsToStack.Num(); Index++)
	{
		AItem* CurrentItem = ItemsToStack[Index];
		
		if (CurrentItem)
		{
			AConsumableItem* ConsumableItem = Cast<AConsumableItem>(CurrentItem);

			if (ConsumableItem)
			{
				int32* CurrentCount = ItemsCount.Find(ConsumableItem->ItemID);

				if (CurrentCount)
				{
					if (*CurrentCount >= ConsumableItem->MaxCount)
					{
						*CurrentCount -= ConsumableItem->MaxCount;
						ConsumableItem->Count = ConsumableItem->MaxCount;
					}
					else if (*CurrentCount > 0)
					{
						ConsumableItem->Count = *CurrentCount;
						*CurrentCount = 0;
					}
					else if (*CurrentCount == 0)
					{
						ItemsToStack[Index] = NULL;
						RemoveItemFromInventory(CurrentItem, true);
					}
				}
			}
		}
	}

	for (int32 Index = 0; Index < ItemsToStack.Num(); Index++)
	{
		Inventory[Index] = ItemsToStack[Index];
		
		if(Inventory[Index])
			Inventory[Index]->CurrentSlotIndex = Index;
	}
}


bool UInventory::ServerAutoStack_Validate()
{
	return true;
}



void UInventory::ServerUnlockSlots_Implementation(uint8 NumberOfSlotsToUnlock)
{	
	if (NumberOfSlotsToUnlock <= NumberOfLockedSlots)
	{
		for (int32 Count = 0; Count < NumberOfSlotsToUnlock; Count++)
		{
			Inventory.Add(NULL);
			NumberOfLockedSlots--;
		}
	}

	ClientUnlockSlots(NumberOfSlotsToUnlock);
}


bool UInventory::ServerUnlockSlots_Validate(uint8 NumberOfSlotsToUnlock)
{
	return true;
}


void UInventory::ClientUnlockSlots_Implementation(uint8 NumberOfSlotsToUnlock)
{
	APlayerController* CurrentController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	ATheEternalTowerPlayerController* MyPlayerController = Cast<ATheEternalTowerPlayerController>(CurrentController);
	
	
	uint8 LastInventoryIndex = NumberOfSlots - NumberOfLockedSlots;

	for (uint8 Count = LastInventoryIndex ; Count < LastInventoryIndex + NumberOfSlotsToUnlock; Count++)
	{
		MyPlayerController->UpdateSlotLockStatus(Count, false);
		NumberOfLockedSlots--;
	}

	if (MyPlayerController->InventoryGridPanel)
		MyPlayerController->UpdateInventoryGrid();
}


void UInventory::OnRep_UpdateInventory()
{
	APlayerController* CurrentController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	ATheEternalTowerPlayerController* MyPlayerController = Cast<ATheEternalTowerPlayerController>(CurrentController);

	
	if (MyPlayerController->InventoryGridPanel)
	{
		NumberOfSlots = Inventory.Num() + NumberOfLockedSlots;
		MyPlayerController->UpdateInventoryGrid();
	}
}


void UInventory::OnRep_UpdateMoney()
{
	APlayerController* CurrentController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	ATheEternalTowerPlayerController* MyPlayerController = Cast<ATheEternalTowerPlayerController>(CurrentController);

	MyPlayerController->UpdateGold();
}


void UInventory::DeactivateItem(int32 ItemID, float Cooldown)
{
	ServerItemsOnCooldown.Add(ItemID, Cooldown);

	for (auto& Item : Inventory)
	{
		if (!Item)
			continue;

		if (Item->ItemID == ItemID)
		{
			ABuffConsumable* BuffItem = Cast<ABuffConsumable>(Item);

			if (BuffItem)
			{
				BuffItem->bOnCooldown = true;
				BuffItem->ServerRemainingCooldown = Cooldown;
			}
		}
	}

	FTimerHandle ItemCooldownHandle;
	FTimerDelegate ItemCooldownDelegate;

	ItemCooldownDelegate.BindUFunction(this, FName("ReactivateItem"), ItemID, Cooldown);

	GetWorld()->GetTimerManager().SetTimer(ItemCooldownHandle, ItemCooldownDelegate, Cooldown, false);
}



void UInventory::ReactivateItem(int32 ItemID, float Cooldown)
{
	ServerItemsOnCooldown.Remove(ItemID);

	for (auto& Item : Inventory)
	{
		if (!Item)
			continue;

		if (Item->ItemID == ItemID)
		{
			ABuffConsumable* BuffItem = Cast<ABuffConsumable>(Item);

			if (BuffItem)
				BuffItem->bOnCooldown = false;
		}
	}
}


void UInventory::TickComponent(float Deltatime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (GetOwner()->HasAuthority())
	{
		for (auto& Element : ServerItemsOnCooldown)
		{
			if (Element.Value > 0.0f)
				Element.Value -= Deltatime;
			else
				Element.Value = 0.0f;
		}
	}
}