// Fill out your copyright notice in the Description page of Project Settings.

#include "TheEternalTower.h"
#include "TheEternalTowerCharacter.h"
#include "TheEternalTowerPlayerController.h"
#include "UnrealNetwork.h"
#include "BuffConsumable.h"
#include "Engine.h"


ABuffConsumable::ABuffConsumable()
{
	PrimaryActorTick.bCanEverTick = true;
	bIsPermanent = false;
	Duration = 0.0f;
	Cooldown = 0.0f;
	bOnCooldown = false;
	ServerRemainingCooldown = 0.0f;
	ClientRemainingCooldown = 0.0f;
	bIsActive = false;
	ServerRemainingTime = 0.0f;
	ClientRemainingTime = 0.0f;
}

void ABuffConsumable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABuffConsumable, bOnCooldown, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ABuffConsumable, ServerRemainingCooldown, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(ABuffConsumable, bIsActive, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ABuffConsumable, ServerRemainingTime, COND_OwnerOnly);
}


void ABuffConsumable::ServerUseItem_Implementation()
{
	ATheEternalTowerCharacter* MyCharacter = Cast<ATheEternalTowerCharacter>(GetOwner());

	if (!MyCharacter)
		return;
	
	if (MyCharacter->MyInventory->ServerItemsOnCooldown.Contains(ItemID))
		return;

	bool bUsed = false;

	if (bIsPermanent)
	{
		for (auto& Attribute : AttributesToBoost)
		{
			switch (Attribute.AffectedAttribute)
			{
			case EAttribute::ATTRIB_EXP:
			{				
				MyCharacter->ServerAddExperience(Attribute.Value);
				bUsed = true;
				
				break;
			}
			case EAttribute::ATTRIB_HEALTH:
			{
				if (Attribute.bIsPercentile)
				{
					float Multiplier = Attribute.Value / 100.0f;
					float HealthToAdd = (MyCharacter->MaximumHealth + MyCharacter->BonusMaximumHealth) * Multiplier;
					MyCharacter->ServerAddHealth(HealthToAdd);
				}
				else
				{
					MyCharacter->ServerAddHealth(Attribute.Value);
				}
				
				bUsed = true;

				break;
			}
			case EAttribute::ATTRIB_MANA:
			{
				MyCharacter->ServerAddMana(Attribute.Value);
				bUsed = true;

				break;
			}
			default:
			{
				bUsed = false;

				break;
			}
			}
		}
	}
	else
	{
		ABuffConsumable* ExistingBuff = NULL;
		
		for (auto& BuffItem : MyCharacter->ActiveBuffItems)
		{
			if (BuffItem->ItemID == ItemID)
			{
				ExistingBuff = BuffItem;
				break;
			}
		}
		
		if (ExistingBuff)
		{
			GetWorldTimerManager().ClearTimer(ExistingBuff->BuffTimerHandle);
			FTimerManager::ValidateHandle(ExistingBuff->BuffTimerHandle);
			
			ExistingBuff->ServerRemainingTime = 0.0f;
			ExistingBuff->bIsActive = false;
			
			MyCharacter->ActiveBuffItems.Remove(ExistingBuff);

			if (ExistingBuff->CurrentSlotIndex == -1)
			{
				ExistingBuff->Destroy();
			}
		}
		else
		{
			MyCharacter->ServerBuffAttributes(AttributesToBoost);
		}

		GetWorldTimerManager().SetTimer(BuffTimerHandle, this, &ABuffConsumable::EndBuff, Duration, false);
		
		ServerRemainingTime = Duration;
		ClientUpdateTime(ServerRemainingTime);

		MyCharacter->ActiveBuffItems.Add(this);
		
		bIsActive = true;
		
		bUsed = true;
	}

	if (bUsed)
	{
		ClientItemMoved();
		MyCharacter->MyInventory->DeactivateItem(ItemID, Cooldown);

		if (bIsStackable)
		{
			if (Count - 1 <= 0)
			{
				if (MyCharacter)
				{
					if (bIsPermanent)
					{
						MyCharacter->MyInventory->RemoveItemFromInventory(this, true);
					}
					else
					{
						MyCharacter->MyInventory->RemoveItemFromInventory(this, false);
						Count--;
					}
				}
			}
			else
			{
				Count--;
			}
		}
		else
		{
			if (MyCharacter)
			{
				if (bIsPermanent)
				{
					MyCharacter->MyInventory->RemoveItemFromInventory(this, true);
				}
				else
				{
					MyCharacter->MyInventory->RemoveItemFromInventory(this, false);
				}
			}
		}

		ATheEternalTowerPlayerController* MyController = Cast<ATheEternalTowerPlayerController>(MyCharacter->GetController());
		if (MyController)
		{
			MyController->ClientUpdateHotbarLayout();
		}
	}

}


void ABuffConsumable::OnRep_UpdateCooldown()
{
	if (bOnCooldown)
	{
		if (ServerRemainingCooldown > 0)
		{
			ClientRemainingCooldown = ServerRemainingCooldown;
		}
		else
		{
			ClientRemainingCooldown = Cooldown;
		}
	}
	else
	{
		ClientRemainingCooldown = 0.0f;
	}
}


void ABuffConsumable::OnRep_UpdateDuration()
{
	if (bIsActive)
	{
		if(ServerRemainingTime > 0)
		{
			ClientRemainingTime = ServerRemainingTime;
		}
		else
		{
			ClientRemainingTime = Duration;
		}
	}
	else
	{
		ClientRemainingTime = 0.0f;
	}
}


void ABuffConsumable::EndBuff()
{
	ATheEternalTowerCharacter* MyCharacter = Cast<ATheEternalTowerCharacter>(GetOwner());

	if (!MyCharacter)
		return;
	
	ServerRemainingTime = 0.0f;
	bIsActive = false;
	
	MyCharacter->ActiveBuffItems.Remove(this);
	MyCharacter->ServerRemoveBuff(AttributesToBoost);

	if (CurrentSlotIndex == -1)
	{
		Destroy();
	}
}


void ABuffConsumable::ServerForceBuffEnd_Implementation()
{
	ATheEternalTowerCharacter* MyCharacter = Cast<ATheEternalTowerCharacter>(GetOwner());

	if (!MyCharacter)
		return;

	GetWorldTimerManager().ClearTimer(BuffTimerHandle);
	FTimerManager::ValidateHandle(BuffTimerHandle);
	
	ServerRemainingTime = 0.0f;
	bIsActive = false;
	
	MyCharacter->ActiveBuffItems.Remove(this);
	MyCharacter->ServerRemoveBuff(AttributesToBoost);

	if (CurrentSlotIndex == -1)
	{
		Destroy();
	}
}


bool ABuffConsumable::ServerForceBuffEnd_Validate()
{
	return true;
}


void ABuffConsumable::ClientUpdateTime_Implementation(float NewTime)
{
	ClientRemainingTime = NewTime;
}


void ABuffConsumable::ClientUsedItem_Implementation(bool bUsedSuccessfully)
{
}





void ABuffConsumable::Tick(float DeltaTime)
{
	if (HasAuthority())
	{
		if (bOnCooldown)
		{
			if (ServerRemainingCooldown > 0.0f)
				ServerRemainingCooldown -= DeltaTime;
			else
				ServerRemainingCooldown = 0.0f;
		}
		
		if (bIsActive)
		{
			if (ServerRemainingTime > 0.0f)
				ServerRemainingTime -= DeltaTime;
			else
				ServerRemainingTime = 0.0f;
		}
	}
	else
	{
		if (bOnCooldown)
		{
			if (ClientRemainingCooldown > 0.0f)
				ClientRemainingCooldown -= DeltaTime;
			else
				ClientRemainingCooldown = 0.0f;
		}

		if (bIsActive)
		{
			if (ClientRemainingTime > 0.0f)
				ClientRemainingTime -= DeltaTime;
			else
				ClientRemainingTime = 0.0f;
		}
	}
}