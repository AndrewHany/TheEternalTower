// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Inventory/ConsumableItem.h"
#include "BuffConsumable.generated.h"

/**
 * 
 */
UCLASS()
class THEETERNALTOWER_API ABuffConsumable : public AConsumableItem
{
	GENERATED_BODY()
	
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff Properties")
	bool bIsPermanent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff Properties", meta = (EditCondition = "!bIsPermanent"))
	float Duration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff Properties")
	float Cooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff Properties|Attributes To Buff")
	TArray<FAttributeManipulation> AttributesToBoost;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_UpdateCooldown)
	bool bOnCooldown;

	UPROPERTY(BlueprintReadWrite, Replicated)
	float ServerRemainingCooldown;

	UPROPERTY(BlueprintReadWrite)
	float ClientRemainingCooldown;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_UpdateDuration)
	bool bIsActive;

	UPROPERTY(BlueprintReadWrite, Replicated)
	float ServerRemainingTime;

	UPROPERTY(BlueprintReadWrite)
	float ClientRemainingTime;

	UPROPERTY(BlueprintReadWrite)
	FTimerHandle BuffTimerHandle;

public:

	ABuffConsumable();

	virtual void ServerUseItem_Implementation() override;

	UFUNCTION()
	void OnRep_UpdateCooldown();

	UFUNCTION()
	void OnRep_UpdateDuration();

	UFUNCTION()
	void EndBuff();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ServerForceBuffEnd();
	void ServerForceBuffEnd_Implementation();
	bool ServerForceBuffEnd_Validate();

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientUpdateTime(float NewTime);
	void ClientUpdateTime_Implementation(float NewTime);

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Buff Effect")
	void ClientUsedItem(bool bUsedSuccessfully);
	void ClientUsedItem_Implementation(bool bUsedSuccessfully);

	virtual void Tick(float DeltaTime) override;

};
