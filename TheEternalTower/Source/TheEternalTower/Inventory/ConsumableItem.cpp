// Fill out your copyright notice in the Description page of Project Settings.

#include "TheEternalTower.h"
#include "ConsumableItem.h"
#include "UnrealNetwork.h"
#include "TheEternalTowerPlayerController.h"


AConsumableItem::AConsumableItem()
{
	bReplicates = true;

	ConsumableEffect = EConsumableType::TYPE_NONE;
	bIsStackable = false;
	Count = 1;
	MaxCount = 1;
	bIsInHotbar = false;
}

// Replication
void AConsumableItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AConsumableItem, bIsStackable, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AConsumableItem, Count,		   COND_OwnerOnly);
}


void AConsumableItem::OnRep_CountChange()
{
	APlayerController* CurrentController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	ATheEternalTowerPlayerController* MyPlayerController = Cast<ATheEternalTowerPlayerController>(CurrentController);

	if (MyPlayerController->InventoryGridPanel)
	{
		MyPlayerController->RedrawInventorySlot(CurrentSlotIndex);

		if (bIsInHotbar)
			MyPlayerController->UpdateHotbarSlots();
	}
}


FCurrency AConsumableItem::GetTotalPrice()
{
	if(bIsStackable && Count > 1)
		return SalePrice * Count;

	return SalePrice;
}