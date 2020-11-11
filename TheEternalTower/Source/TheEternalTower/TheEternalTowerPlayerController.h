// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "UMG.h"
#include "GameFramework/Actor.h"
#include "TheEternalTowerPlayerController.generated.h"


UCLASS()
class THEETERNALTOWER_API ATheEternalTowerPlayerController : public APlayerController
{
	GENERATED_BODY()

// PROPERTIES
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	TSubclassOf<class UUserWidget> HUDWidget;

	UPROPERTY(BlueprintReadWrite)
	UUserWidget* GameHUD;

	UPROPERTY(BlueprintReadWrite)
	UUserWidget* MyInventoryWidget;

	UPROPERTY(BlueprintReadWrite)
	UUserWidget* MyCharacterWidget;

	UPROPERTY(BlueprintReadWrite)
	UUserWidget* MyHotbarWidget;

	UPROPERTY(BlueprintReadWrite)
	UUserWidget* MyActiveBuffsWidget;

	UPROPERTY(BlueprintReadWrite)
	UCanvasPanel* HUDCanvasPanel;

	UPROPERTY(BlueprintReadWrite, Category = "Inventory UI")
	UUniformGridPanel* InventoryGridPanel;

	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> HotbarLayout;

// FUNCTIONS
public:
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// UI Functions, Implemented in blueprints
	UFUNCTION(BlueprintImplementableEvent, Category = "UI Management")
	void RedrawInventorySlot(int32 SlotIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI Management")
	void UpdateInventoryGrid();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI Management")
	void UpdateHotbarSlots();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI Management")
	void UpdateActiveBuffs();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "UI Management")
	void UpdateSlotLockStatus(uint8 SlotIndex, bool bIsLocked);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI Management")
	void UpdateGold();


	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ServerUpdateHotbarLayout(int32 HotbarSlotIndex, AActor* AssignedActor);
	void ServerUpdateHotbarLayout_Implementation(int32 HotbarSlotIndex, AActor* AssignedActor);
	bool ServerUpdateHotbarLayout_Validate(int32 HotbarSlotIndex, AActor* AssignedActor);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientUpdateHotbarLayout();
	void ClientUpdateHotbarLayout_Implementation();
};
