// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "TheEternalTowerCharacter.h"
#include "Party.generated.h"


UCLASS()
class THEETERNALTOWER_API AParty : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AParty();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party Settings")
		uint8 ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party Settings")
		uint8 Size;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party Settings")
		uint8 MaxSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party Settings")
		ATheEternalTowerCharacter* Leader;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party Settings")
		TArray<ATheEternalTowerCharacter*> Players;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Party Management")
		void Initialize(ATheEternalTowerCharacter* Lead, uint8 PartyID);

	UFUNCTION(BlueprintCallable, Category = "Party Management")
		void AddPlayer(ATheEternalTowerCharacter* Character);

	UFUNCTION(BlueprintCallable, Category = "Party Management")
		void RemovePlayer(ATheEternalTowerCharacter* Character);

	UFUNCTION(BlueprintCallable, Category = "Party Management")
		bool Full();

	UFUNCTION(BlueprintCallable, Category = "Party Management")
		void Destroy();


};
