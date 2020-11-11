// Fill out your copyright notice in the Description page of Project Settings.

#include "TheEternalTower.h"
#include "Engine.h"
#include "UnrealNetwork.h"
#include "Item.h"


// Sets default values
AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = false;
	SetActorHiddenInGame(true);
	bReplicates = true;

	ItemID			  =	-1;
	ItemName		  =	"";
	ItemRarity		  =	EItemRarity::RARITY_COMMON;
	RequiredLevel	  =	1;
	FlavorText		  =	"";
	EffectText		  =	"";
	CurrentSlotIndex  = -1;
	SalePrice = FCurrency(0, 0, 0);
}

void AItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AItem, CurrentSlotIndex, COND_OwnerOnly);
}


FLinearColor AItem::GetColorBasedOnRarity()
{
	switch (ItemRarity)
	{
	case EItemRarity::RARITY_COMMON:
		return COMMON_COLOR;
	case EItemRarity::RARITY_RARE:
		return RARE_COLOR;
	case EItemRarity::RARITY_EPIC:
		return EPIC_COLOR;
	case EItemRarity::RARITY_LEGENDARY:
		return LEGENDARY_COLOR;
	case EItemRarity::RARITY_GODLIKE:
		return GODLIKE_COLOR;
	default:
		return COMMON_COLOR;
	}
}


// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
}


void AItem::ServerUseItem_Implementation()
{
}


bool AItem::ServerUseItem_Validate()
{
	return true;
}


void AItem::ClientItemMoved_Implementation()
{
	if(HandleSoundEffect)
		UGameplayStatics::PlaySound2D(GetWorld(), HandleSoundEffect);
}