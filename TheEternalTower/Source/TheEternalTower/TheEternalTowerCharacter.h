// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
#include "Inventory/Inventory.h"
#include "Inventory/EquippableItem.h"
#include "TheEternalTowerCharacter.generated.h"


UCLASS(config=Game)
class ATheEternalTowerCharacter : public ACharacter
{
	GENERATED_BODY()

// PROPERTIES
public:

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Text")
	class UTextRenderComponent* PlayerStats;

	// Character Attributes

	/** The character's current level based on experience reached.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	uint8 Level;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attributes")
	uint8 LevelCap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
	ECharacterClass CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Admin")
	bool bIsGameMaster;

	/** The character's current experience points.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	int32 CurrentExperience;

	/** The needed experience points to reach the next level.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	int32 ExperienceToNextLevel;

	/** The character's maximum health.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float MaximumHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float BonusMaximumHealth;

	/** The character's current hit points.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float Health;

	/** The character's maximum mana.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float MaximumMana;

	/** The character's current mana points.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float Mana;

	/** The character's current minimum physical attack power.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float MinimumPhysicalAttackPower;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float BonusMinimumPhysicalAttackPower;

	/** The character's current maximum physical attack power.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float MaximumPhysicalAttackPower;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float BonusMaximumPhysicalAttackPower;

	/** The character's current minimum magical attack power.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float MinimumMagicalAttackPower;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float BonusMinimumMagicalAttackPower;

	/** The character's current maximum magical attack power.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float MaximumMagicalAttackPower;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float BonusMaximumMagicalAttackPower;

	/** The character's current accuracy as a percentage, this determines how successful an attack is.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float Accuracy;

	/** The character's current physical defense.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float PhysicalDefense;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float BonusPhysicalDefense;

	/** The character's current magical defense.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float MagicalDefense;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float BonusMagicalDefense;

	/** The character's current evasion as a percentage, this determines how successful a dodge is.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float Evasion;

	/** The character's current damage reduction percentage.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float DamageReduction;

	/** The character's current life steal percentage.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float LifeSteal;

	/** The character's current critical hit rate percentage, this determines how often critical damage would be applied*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float CriticalRate;

	/** The character's current critical damage multiplier.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float CriticalDamage;

	/** The character's current chance value, this changes daily for every character on the server.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	int32 Luck;

	/** The character's current critical damage multiplier.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Attributes")
	float MovementSpeed;



	// Active buff items
	UPROPERTY(ReplicatedUsing = OnRep_UpdateActiveBuffs, BlueprintReadWrite, Category = "Active Buffs")
	TArray<ABuffConsumable*> ActiveBuffItems;

	//Equipment

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_UpdateLeftEarring, Category = "Equipment")
	AEquippableItem* LeftEarring;
	
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_UpdateRightEarring, Category = "Equipment")
	AEquippableItem* RightEarring;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_UpdateHelmet, Category = "Equipment")
	AEquippableItem* Helmet;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_UpdateNecklace, Category = "Equipment")
	AEquippableItem* Necklace;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_UpdateArmor, Category = "Equipment")
	AEquippableItem* Armor;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_UpdateLeftRing, Category = "Equipment")
	AEquippableItem* LeftRing;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_UpdateRightRing, Category = "Equipment")
	AEquippableItem* RightRing;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_UpdateWeapon, Category = "Equipment")
	AEquippableItem* Weapon;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_UpdateGloves, Category = "Equipment")
	AEquippableItem* Gloves;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_UpdateBoots, Category = "Equipment")
	AEquippableItem* Boots;


	// base body
	UPROPERTY(EditAnywhere, Category = "Equipment")
	USkeletalMesh* BaseSkeletalMesh;

	// inventory
	UPROPERTY(BlueprintReadWrite)
	UInventory* MyInventory;

public:
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	FTimerHandle DebugTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Data Tables")
	UDataTable* LevelUpDataTable;


// FUNCTIONS
private:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** Called via input to turn at a given rate.
	Rate is a normalized rate, i.e. 1.0 means 100% of desired turn rate */
	void TurnAtRate(float Rate);

	/** Called via input to turn look up/down at a given rate.
	Rate is a normalized rate, i.e. 1.0 means 100% of desired turn rate */
	void LookUpAtRate(float Rate);

	// Override the input component to bind any desired function to a key event
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** A function that increases the character's level,
	grabs the bonus stats from the level up datatable and
	updates the character's attributes.*/
	void ServerLevelUp(int32 LeftOverExp);


public:

	ATheEternalTowerCharacter();

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** A function that adds experience to the character
	and levels it up if it has reached the level's required
	experience threshold*/
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ServerAddExperience				(int32 XPToAdd);
	void ServerAddExperience_Implementation	(int32 XPToAdd);
	bool ServerAddExperience_Validate		(int32 XPToAdd);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ServerAddHealth				(float HealthToAdd);
	void ServerAddHealth_Implementation	(float HealthToAdd);
	bool ServerAddHealth_Validate		(float HealthToAdd);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ServerAddMana					(float ManaToAdd);
	void ServerAddMana_Implementation	(float ManaToAdd);
	bool ServerAddMana_Validate			(float ManaToAdd);

	UFUNCTION(BlueprintCallable)
	void ServerBuffAttributes(TArray<FAttributeManipulation> AttributesToBoost, bool bRecalculate = false);

	UFUNCTION(BlueprintCallable)
	void ServerRemoveBuff(TArray<FAttributeManipulation> BoostedAttributes);

	UFUNCTION(BlueprintCallable)
	void ServerRecalculateBuffs();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ServerEquipItem				(AEquippableItem* ItemToEquip, int32 PlacedLeft = -1);
	void ServerEquipItem_Implementation	(AEquippableItem* ItemToEquip, int32 PlacedLeft = -1);
	bool ServerEquipItem_Validate		(AEquippableItem* ItemToEquip, int32 PlacedLeft = -1);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ServerUnequipItem					(AEquippableItem* ItemToUnequip, bool UnequipLeft = false, int32 SlotToUnequipInto = -1);
	void ServerUnequipItem_Implementation	(AEquippableItem* ItemToUnequip, bool UnequipLeft = false, int32 SlotToUnequipInto = -1);
	bool ServerUnequipItem_Validate			(AEquippableItem* ItemToUnequip, bool UnequipLeft = false, int32 SlotToUnequipInto = -1);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastEquipArmor				(ATheEternalTowerCharacter* Character, USkeletalMesh* NewArmoredMesh);
	void MulticastEquipArmor_Implementation (ATheEternalTowerCharacter* Character, USkeletalMesh* NewArmoredMesh);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastEquipWeapon				(ATheEternalTowerCharacter* Character, AEquippableItem* OldWeapon, AEquippableItem* NewWeapon = NULL);
	void MulticastEquipWeapon_Implementation(ATheEternalTowerCharacter* Character, AEquippableItem* OldWeapon, AEquippableItem* NewWeapon = NULL);


	UFUNCTION(BlueprintImplementableEvent, Category = "Update Equipment")
	void OnRep_UpdateLeftEarring();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Update Equipment")
	void OnRep_UpdateRightEarring();

	UFUNCTION(BlueprintImplementableEvent, Category = "Update Equipment")
	void OnRep_UpdateHelmet();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Update Equipment")
	void OnRep_UpdateNecklace();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Update Equipment")
	void OnRep_UpdateArmor();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Update Equipment")
	void OnRep_UpdateLeftRing();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Update Equipment")
	void OnRep_UpdateRightRing();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Update Equipment")
	void OnRep_UpdateWeapon();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Update Equipment")
	void OnRep_UpdateGloves();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Update Equipment")
	void OnRep_UpdateBoots();



	UFUNCTION(BlueprintImplementableEvent, Category = "Update Equipment")
	void OnRep_UpdateActiveBuffs();


	void DebugFunction(); 
	void DebugFunction2();

	virtual void BeginPlay() override;
};