// Fill out your copyright notice in the Description page of Project Settings.

#include "TheEternalTower.h"
#include "TheEternalTowerPlayerController.h"
#include "Engine.h"
#include "Blueprint/UserWidget.h"


void ATheEternalTowerPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		bShowMouseCursor = true;
		SetInputMode(FInputModeGameAndUI());

		if (HUDWidget)
		{
			GameHUD = CreateWidget<UUserWidget>(this, HUDWidget);
			if (GameHUD)
			{
				GameHUD->AddToViewport();
			}

			MyInventoryWidget = (UUserWidget*)GameHUD->GetWidgetFromName("Inventory");
			if (MyInventoryWidget)
			{
				MyInventoryWidget->SetVisibility(ESlateVisibility::Hidden);
			}

			MyCharacterWidget = (UUserWidget*)GameHUD->GetWidgetFromName("Character");
			if (MyCharacterWidget)
			{
				MyCharacterWidget->SetVisibility(ESlateVisibility::Hidden);
			}

			MyHotbarWidget = (UUserWidget*)GameHUD->GetWidgetFromName("Hotbar");
			
			MyActiveBuffsWidget = (UUserWidget*)GameHUD->GetWidgetFromName("ActiveBuffs");

			HUDCanvasPanel = (UCanvasPanel*)GameHUD->GetWidgetFromName("Canvas");
		}
	}

	if (HasAuthority())
	{
		HotbarLayout.Init(NULL, 20);
	}
}


void ATheEternalTowerPlayerController::ServerUpdateHotbarLayout_Implementation(int32 HotbarSlotIndex, AActor* AssignedActor)
{
	HotbarLayout[HotbarSlotIndex - 1] = AssignedActor;
}


bool ATheEternalTowerPlayerController::ServerUpdateHotbarLayout_Validate(int32 HotbarSlotIndex, AActor* AssignedActor)
{
	return true;
}


void ATheEternalTowerPlayerController::ClientUpdateHotbarLayout_Implementation()
{
	UpdateHotbarSlots();
}


void ATheEternalTowerPlayerController::Tick(float DeltaTime)
{

}