// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "TheEternalTower.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "TheEternalTowerCharacter.h"
#include "TheEternalTowerPlayerController.h"
#include "TheEternalTowerGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Structs/LevelUp.h"
#include "UnrealNetwork.h"
#include "Inventory/Inventory.h"
#include "Inventory/EquippableItem.h"
#include "Engine.h"

//////////////////////////////////////////////////////////////////////////
// ATheEternalTowerCharacter


ATheEternalTowerCharacter::ATheEternalTowerCharacter()
{
	// Activate replication
	bReplicates = true;
	bReplicateMovement = true; 
	bNetUseOwnerRelevancy = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Debug Stats
	PlayerStats = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PlayerStats"));
	PlayerStats->SetRelativeLocation(FVector(0, 100, 100));
	PlayerStats->SetupAttachment(GetRootComponent());

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	// Initializing Attributes
	Level = 1;	
	LevelCap = 10;
	CharacterClass = ECharacterClass::CLASS_WARRIOR;
	CurrentExperience = 0;
	ExperienceToNextLevel = 400;
	MaximumHealth = 100.0f;
	Health = MaximumHealth;
	MaximumMana = 10.0f;
	Mana = MaximumMana;
	MinimumPhysicalAttackPower = 10.0f;
	MaximumPhysicalAttackPower = 100.0f;
	MinimumMagicalAttackPower  = 10.0f;
	MaximumMagicalAttackPower  = 100.0f;
	Accuracy = 90.0f;
	PhysicalDefense = 0.0f;
	MagicalDefense  = 0.0f;
	Evasion = 5.0f;
	DamageReduction = 5.0f;
	LifeSteal = 0.0f;
	CriticalRate = 15.0f;
	CriticalDamage = 120.0f;
	Luck = 0;
	MovementSpeed = GetCharacterMovement()->MaxWalkSpeed;

	// Initializing Bonus Stats
	BonusMaximumHealth = 0.0f;
	BonusMinimumPhysicalAttackPower = 0.0f;
	BonusMaximumPhysicalAttackPower = 0.0f;
	BonusMinimumMagicalAttackPower = 0.0f;
	BonusMaximumMagicalAttackPower = 0.0f;
	BonusPhysicalDefense = 0.0f;
	BonusMagicalDefense = 0.0f;

	// Initializing Equipment
	LeftEarring = NULL;
	RightEarring = NULL;
	Helmet = NULL;
	Necklace = NULL;
	Armor = NULL;
	LeftRing = NULL;
	RightRing = NULL;
	Weapon = NULL;
	Gloves = NULL;
	Boots = NULL;

	bIsGameMaster = false;

	BaseSkeletalMesh = NULL;
	MyInventory = NULL;
}

// Replication
void ATheEternalTowerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATheEternalTowerCharacter, Level);
	DOREPLIFETIME(ATheEternalTowerCharacter, MaximumHealth);
	DOREPLIFETIME(ATheEternalTowerCharacter, BonusMaximumHealth);
	DOREPLIFETIME(ATheEternalTowerCharacter, MaximumMana);
	DOREPLIFETIME(ATheEternalTowerCharacter, Health);
	DOREPLIFETIME(ATheEternalTowerCharacter, Mana);

	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, CurrentExperience, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, ExperienceToNextLevel, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, MinimumPhysicalAttackPower, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, BonusMinimumPhysicalAttackPower, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, MaximumPhysicalAttackPower, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, BonusMaximumPhysicalAttackPower, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, MinimumMagicalAttackPower, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, BonusMinimumMagicalAttackPower, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, MaximumMagicalAttackPower, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, BonusMaximumMagicalAttackPower, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, Accuracy, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, PhysicalDefense, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, BonusPhysicalDefense, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, MagicalDefense, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, BonusMagicalDefense, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, Evasion, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, DamageReduction, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, LifeSteal, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, CriticalRate, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, CriticalDamage, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, Luck, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, MovementSpeed, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, LeftEarring, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, RightEarring, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, Helmet, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, Necklace, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, Armor, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, LeftRing, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, RightRing, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, Weapon, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, Gloves, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, Boots, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(ATheEternalTowerCharacter, ActiveBuffItems, COND_OwnerOnly);
}

// Input
void ATheEternalTowerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Debug Experience System
	PlayerInputComponent->BindAction("Exp_Debug", IE_Released, this, &ATheEternalTowerCharacter::DebugFunction);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATheEternalTowerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATheEternalTowerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATheEternalTowerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATheEternalTowerCharacter::LookUpAtRate);
}


void ATheEternalTowerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}


void ATheEternalTowerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void ATheEternalTowerCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}


void ATheEternalTowerCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}


bool ATheEternalTowerCharacter::ServerAddExperience_Validate(int32 XPToAdd)
{
	return true;
}


void ATheEternalTowerCharacter::ServerAddExperience_Implementation(int32 XPToAdd)
{
	int32 NewExperience = CurrentExperience + XPToAdd;

	if (NewExperience < 0)
		NewExperience = 0;

	if (NewExperience >= ExperienceToNextLevel)
	{
		if (Level != LevelCap)
		{
			CurrentExperience = 0;
			int32 LeftOverExp = NewExperience - ExperienceToNextLevel;

			ServerLevelUp(LeftOverExp);
		}
		else
		{
			CurrentExperience = ExperienceToNextLevel;
		}
	}
	else
	{
		CurrentExperience = NewExperience;
	}
}


void ATheEternalTowerCharacter::ServerLevelUp(int32 LeftOverExp)
{	
	FLevelUp* Row = LevelUpDataTable->FindRow<FLevelUp>(FName(*FString::FromInt(Level + 1)), TEXT(""));

	ExperienceToNextLevel = Row->ExpToNextLevel;

	MinimumPhysicalAttackPower += Row->AddedPhysicalAttack;
	MaximumPhysicalAttackPower += Row->AddedPhysicalAttack;
	MinimumMagicalAttackPower  += Row->AddedMagicalAttack;
	MaximumMagicalAttackPower  += Row->AddedMagicalAttack;

	PhysicalDefense		+= Row->AddedPhysicalDefense;
	MagicalDefense		+= Row->AddedMagicalDefense;
	Accuracy			+= Row->AddedAccuracy;

	Level++;

	MaximumHealth += Row->AddedHealth;
	Health = MaximumHealth;
	MaximumMana   += Row->AddedMana;
	Mana   = MaximumMana;

	if (LeftOverExp > 0)
		ServerAddExperience(LeftOverExp);


	if (MyInventory)
	{
		MyInventory->ServerUnlockSlots(2);
	}


	ServerRecalculateBuffs();
}


void ATheEternalTowerCharacter::ServerAddHealth_Implementation(float HealthToAdd)
{
	float TempHealth = Health + HealthToAdd;

	if (TempHealth >= MaximumHealth + BonusMaximumHealth)
	{
		Health = MaximumHealth + BonusMaximumHealth;
	}
	else
	{
		if (TempHealth > 0.0f)
		{
			Health = TempHealth;
		}
		else
		{
			Health = 0.0f;
			//death
		}
	}
}


bool ATheEternalTowerCharacter::ServerAddHealth_Validate(float HealthToAdd)
{
	return true;
}


void ATheEternalTowerCharacter::ServerAddMana_Implementation(float ManaToAdd)
{
	float TempMana = Mana + ManaToAdd;

	if (TempMana >= MaximumMana)
	{
		Mana = MaximumMana;
	}
	else
	{
		if (TempMana > 0.0f)
		{
			Mana = TempMana;
		}
		else
		{
			Mana = 0.0f;
		}
	}
}


bool ATheEternalTowerCharacter::ServerAddMana_Validate(float ManaToAdd)
{
	return true;
}


void ATheEternalTowerCharacter::ServerBuffAttributes(TArray<FAttributeManipulation> AttributesToBoost, bool bRecalculate)
{
	if (!HasAuthority())
		return;

	float TempBonusMaximumHealth = BonusMaximumHealth;
	
	float TempBonusMinimumPhysicalAttackPower = BonusMinimumPhysicalAttackPower;	
	float TempBonusMaximumPhysicalAttackPower = BonusMaximumPhysicalAttackPower;
	
	float TempBonusMinimumMagicalAttackPower = BonusMinimumMagicalAttackPower;
	float TempBonusMaximumMagicalAttackPower = BonusMaximumMagicalAttackPower;

	float TempAccuracy = Accuracy;
	
	float TempBonusPhysicalDefense = BonusPhysicalDefense;
	float TempBonusMagicalDefense = BonusMagicalDefense;

	float TempEvasion = Evasion;
	float TempDamageReduction = DamageReduction;
	float TempLifeSteal = LifeSteal;

	float TempCriticalRate = CriticalRate;
	float TempCriticalDamage = CriticalDamage;

	float TempLuck = Luck;


	for (auto& Attribute : AttributesToBoost)
	{
		switch (Attribute.AffectedAttribute)
		{
		case EAttribute::ATTRIB_MAX_HEALTH:
		{
			if (Attribute.bIsPercentile)
			{
				float Multiplier = Attribute.Value / 100.0f;
				TempBonusMaximumHealth += MaximumHealth * Multiplier;
			}
			else
			{
				TempBonusMaximumHealth += Attribute.Value;
			}

			break;
		}
		case EAttribute::ATTRIB_MIN_PHYATK:
		{
			if (Attribute.bIsPercentile)
			{
				float Multiplier = Attribute.Value / 100.0f;
				TempBonusMinimumPhysicalAttackPower += MinimumPhysicalAttackPower * Multiplier;
			}
			else
			{
				TempBonusMinimumPhysicalAttackPower += Attribute.Value;
			}

			break;
		}
		case EAttribute::ATTRIB_MAX_PHYATK:
		{
			if (Attribute.bIsPercentile)
			{
				float Multiplier = Attribute.Value / 100.0f;
				TempBonusMaximumPhysicalAttackPower += MaximumPhysicalAttackPower * Multiplier;
			}
			else
			{
				TempBonusMaximumPhysicalAttackPower += Attribute.Value;
			}

			break;
		}
		case EAttribute::ATTRIB_MIN_MAGATK:
		{
			if (Attribute.bIsPercentile)
			{
				float Multiplier = Attribute.Value / 100.0f;
				TempBonusMinimumMagicalAttackPower += MinimumMagicalAttackPower * Multiplier;
			}
			else
			{
				TempBonusMinimumMagicalAttackPower += Attribute.Value;
			}

			break;
		}
		case EAttribute::ATTRIB_MAX_MAGATK:
		{
			if (Attribute.bIsPercentile)
			{
				float Multiplier = Attribute.Value / 100.0f;
				TempBonusMaximumMagicalAttackPower += MaximumMagicalAttackPower * Multiplier;
			}
			else
			{
				TempBonusMaximumMagicalAttackPower += Attribute.Value;
			}

			break;
		}
		case EAttribute::ATTRIB_ACC:
		{
			if (!bRecalculate)
			{
				TempAccuracy += Attribute.Value;
			}

			break;
		}
		case EAttribute::ATTRIB_PHYDEF:
		{
			if (Attribute.bIsPercentile)
			{
				float Multiplier = Attribute.Value / 100.0f;
				TempBonusPhysicalDefense += PhysicalDefense * Multiplier;
			}
			else
			{
				TempBonusPhysicalDefense += Attribute.Value;
			}

			break;
		}
		case EAttribute::ATTRIB_MAGDEF:
		{
			if (Attribute.bIsPercentile)
			{
				float Multiplier = Attribute.Value / 100.0f;
				TempBonusMagicalDefense += MagicalDefense * Multiplier;
			}
			else
			{
				TempBonusMagicalDefense += Attribute.Value;
			}

			break;
		}
		case EAttribute::ATTRIB_EVASION:
		{
			if (!bRecalculate)
			{
				TempEvasion += Attribute.Value;
			}

			break;
		}
		case EAttribute::ATTRIB_DMGREDUCE:
		{
			if (!bRecalculate)
			{
				TempDamageReduction += Attribute.Value;
			}

			break;
		}
		case EAttribute::ATTRIB_LIFESTEAL:
		{
			if (!bRecalculate)
			{
				TempLifeSteal += Attribute.Value;
			}

			break;
		}
		case EAttribute::ATTRIB_CRITRATE:
		{
			if (!bRecalculate)
			{
				TempCriticalRate += Attribute.Value;
			}

			break;
		}
		case EAttribute::ATTRIB_CRITDMG:
		{
			if (!bRecalculate)
			{
				TempCriticalDamage += Attribute.Value;
			}

			break;
		}
		case EAttribute::ATTRIB_LUCK:
		{
			if (!bRecalculate)
			{
				TempLuck += Attribute.Value;
			}

			break;
		}
		case EAttribute::ATTRIB_MOVESPD:
		{
			if (!bRecalculate)
			{
				MovementSpeed += Attribute.Value;
				GetCharacterMovement()->MaxWalkSpeed += Attribute.Value;
			}

			break;
		}
		default:
			break;
		}
	}

	BonusMaximumHealth = TempBonusMaximumHealth;
	
	if (Health > MaximumHealth + BonusMaximumHealth)
	{
		Health = MaximumHealth + BonusMaximumHealth;
	}

	BonusMinimumPhysicalAttackPower = TempBonusMinimumPhysicalAttackPower;
	BonusMaximumPhysicalAttackPower = TempBonusMaximumPhysicalAttackPower;

	BonusMinimumMagicalAttackPower = TempBonusMinimumMagicalAttackPower;
	BonusMaximumMagicalAttackPower = TempBonusMaximumMagicalAttackPower;

	Accuracy = TempAccuracy;

	BonusPhysicalDefense = TempBonusPhysicalDefense;
	BonusMagicalDefense = TempBonusMagicalDefense;

	Evasion = TempEvasion;
	DamageReduction = TempDamageReduction;
	LifeSteal = TempLifeSteal;

	CriticalRate = TempCriticalRate;
	CriticalDamage = TempCriticalDamage;

	Luck = TempLuck;
}


void ATheEternalTowerCharacter::ServerRemoveBuff(TArray<FAttributeManipulation> BoostedAttributes)
{
	if (!HasAuthority())
		return;

	float TempBonusMaximumHealth = BonusMaximumHealth;

	float TempBonusMinimumPhysicalAttackPower = BonusMinimumPhysicalAttackPower;
	float TempBonusMaximumPhysicalAttackPower = BonusMaximumPhysicalAttackPower;

	float TempBonusMinimumMagicalAttackPower = BonusMinimumMagicalAttackPower;
	float TempBonusMaximumMagicalAttackPower = BonusMaximumMagicalAttackPower;

	float TempAccuracy = Accuracy;

	float TempBonusPhysicalDefense = BonusPhysicalDefense;
	float TempBonusMagicalDefense = BonusMagicalDefense;

	float TempEvasion = Evasion;
	float TempDamageReduction = DamageReduction;
	float TempLifeSteal = LifeSteal;

	float TempCriticalRate = CriticalRate;
	float TempCriticalDamage = CriticalDamage;

	float TempLuck = Luck;


	for (auto& Attribute : BoostedAttributes)
	{
		switch (Attribute.AffectedAttribute)
		{
		case EAttribute::ATTRIB_MAX_HEALTH:
		{
			if (Attribute.bIsPercentile)
			{
				float Multiplier = Attribute.Value / 100.0f;
				TempBonusMaximumHealth -= MaximumHealth * Multiplier;
			}
			else
			{
				TempBonusMaximumHealth -= Attribute.Value;
			}

			break;
		}
		case EAttribute::ATTRIB_MIN_PHYATK:
		{
			if (Attribute.bIsPercentile)
			{
				float Multiplier = Attribute.Value / 100.0f;
				TempBonusMinimumPhysicalAttackPower -= MinimumPhysicalAttackPower * Multiplier;
			}
			else
			{
				TempBonusMinimumPhysicalAttackPower -= Attribute.Value;
			}

			break;
		}
		case EAttribute::ATTRIB_MAX_PHYATK:
		{
			if (Attribute.bIsPercentile)
			{
				float Multiplier = Attribute.Value / 100.0f;
				TempBonusMaximumPhysicalAttackPower -= MaximumPhysicalAttackPower * Multiplier;
			}
			else
			{
				TempBonusMaximumPhysicalAttackPower -= Attribute.Value;
			}

			break;
		}
		case EAttribute::ATTRIB_MIN_MAGATK:
		{
			if (Attribute.bIsPercentile)
			{
				float Multiplier = Attribute.Value / 100.0f;
				TempBonusMinimumMagicalAttackPower -= MinimumMagicalAttackPower * Multiplier;
			}
			else
			{
				TempBonusMinimumMagicalAttackPower -= Attribute.Value;
			}

			break;
		}
		case EAttribute::ATTRIB_MAX_MAGATK:
		{
			if (Attribute.bIsPercentile)
			{
				float Multiplier = Attribute.Value / 100.0f;
				TempBonusMaximumMagicalAttackPower -= MaximumMagicalAttackPower * Multiplier;
			}
			else
			{
				TempBonusMaximumMagicalAttackPower -= Attribute.Value;
			}

			break;
		}
		case EAttribute::ATTRIB_ACC:
		{
			TempAccuracy -= Attribute.Value;

			break;
		}
		case EAttribute::ATTRIB_PHYDEF:
		{
			if (Attribute.bIsPercentile)
			{
				float Multiplier = Attribute.Value / 100.0f;
				TempBonusPhysicalDefense -= PhysicalDefense * Multiplier;
			}
			else
			{
				TempBonusPhysicalDefense -= Attribute.Value;
			}

			break;
		}
		case EAttribute::ATTRIB_MAGDEF:
		{
			if (Attribute.bIsPercentile)
			{
				float Multiplier = Attribute.Value / 100.0f;
				TempBonusMagicalDefense -= MagicalDefense * Multiplier;
			}
			else
			{
				TempBonusMagicalDefense -= Attribute.Value;
			}

			break;
		}
		case EAttribute::ATTRIB_EVASION:
		{
			TempEvasion -= Attribute.Value;

			break;
		}
		case EAttribute::ATTRIB_DMGREDUCE:
		{
			TempDamageReduction -= Attribute.Value;

			break;
		}
		case EAttribute::ATTRIB_LIFESTEAL:
		{
			TempLifeSteal -= Attribute.Value;

			break;
		}
		case EAttribute::ATTRIB_CRITRATE:
		{
			TempCriticalRate -= Attribute.Value;

			break;
		}
		case EAttribute::ATTRIB_CRITDMG:
		{
			TempCriticalDamage -= Attribute.Value;

			break;
		}
		case EAttribute::ATTRIB_LUCK:
		{
			TempLuck -= Attribute.Value;

			break;
		}
		case EAttribute::ATTRIB_MOVESPD:
		{
			MovementSpeed -= Attribute.Value;
			GetCharacterMovement()->MaxWalkSpeed -= Attribute.Value;

			break;
		}
		default:
			break;
		}
	}

	BonusMaximumHealth = TempBonusMaximumHealth;
	
	if (Health > MaximumHealth + BonusMaximumHealth)
	{
		Health = MaximumHealth + BonusMaximumHealth;
	}

	BonusMinimumPhysicalAttackPower = TempBonusMinimumPhysicalAttackPower;
	BonusMaximumPhysicalAttackPower = TempBonusMaximumPhysicalAttackPower;

	BonusMinimumMagicalAttackPower = TempBonusMinimumMagicalAttackPower;
	BonusMaximumMagicalAttackPower = TempBonusMaximumMagicalAttackPower;

	Accuracy = TempAccuracy;

	BonusPhysicalDefense = TempBonusPhysicalDefense;
	BonusMagicalDefense = TempBonusMagicalDefense;

	Evasion = TempEvasion;
	DamageReduction = TempDamageReduction;
	LifeSteal = TempLifeSteal;

	CriticalRate = TempCriticalRate;
	CriticalDamage = TempCriticalDamage;

	Luck = TempLuck;
}


void ATheEternalTowerCharacter::ServerRecalculateBuffs()
{
	if (!HasAuthority())
		return;

	BonusMaximumHealth = 0.0f;
	BonusMinimumPhysicalAttackPower = 0.0f;
	BonusMaximumPhysicalAttackPower = 0.0f;
	BonusMinimumMagicalAttackPower = 0.0f;
	BonusMaximumMagicalAttackPower = 0.0f;
	BonusPhysicalDefense = 0.0f;
	BonusMagicalDefense = 0.0f;

	for (auto& BuffItem : ActiveBuffItems)
	{
		if (BuffItem->bIsActive && BuffItem->ServerRemainingTime > 0)
		{
			ServerBuffAttributes(BuffItem->AttributesToBoost, true);
		}
	}

	// buff skills later
}


void ATheEternalTowerCharacter::DebugFunction()
{
	ServerAddExperience(3000);
}


void ATheEternalTowerCharacter::DebugFunction2()
{
	FString TestString = FString::Printf(TEXT("Level:\t%d\nHealth:\t%.2f/%.2f"), Level, Health, MaximumHealth + BonusMaximumHealth);
	PlayerStats->SetText(FText::FromString(TestString));
}


void ATheEternalTowerCharacter::ServerEquipItem_Implementation(AEquippableItem* ItemToEquip, int32 PlacedLeft)
{
	if (!ItemToEquip)
	{
		return;
	}
		
	if (ItemToEquip->RequiredLevel > Level)
	{
		return;
	}

	if (ItemToEquip->bClassSpecific)
	{
		if (ItemToEquip->EquipmentClass != CharacterClass)
		{
			return;
		}
	}

	bool bEquipped = false;
		
	switch (ItemToEquip->EquipmentType)
	{
	case EEquipmentType::EQUIPMENT_WEAPON:
	{
		AEquippableItem* UnequippedWeapon = NULL;
		
		if (Weapon)
		{
			UnequippedWeapon = Weapon;
			ServerUnequipItem(Weapon);
		}	

		if (!Weapon)
		{
			Weapon = ItemToEquip;

			MulticastEquipWeapon(this, UnequippedWeapon, Weapon);

			bEquipped = true;
		}

		break;
	}
	case EEquipmentType::EQUIPMENT_HELMET:
	{
		if (Helmet)
		{
			ServerUnequipItem(Helmet);
		}

		if (!Helmet)
		{
			Helmet = ItemToEquip;
			bEquipped = true;
		}

		break;
	}
	case EEquipmentType::EQUIPMENT_ARMOR:
	{
		if (Armor)
		{
			ServerUnequipItem(Armor);
		}

		if (!Armor)
		{
			Armor = ItemToEquip;
			
			if (Armor->SkeletalMesh)
			{
				MulticastEquipArmor(this, Armor->SkeletalMesh);
			}
			else
			{
				if (BaseSkeletalMesh)
					MulticastEquipArmor(this, BaseSkeletalMesh);
			}
			
			bEquipped = true;
		}

		break;
	}
	case EEquipmentType::EQUIPMENT_GLOVES:
	{
		if (Gloves)
		{
			ServerUnequipItem(Gloves);
		}

		if (!Gloves)
		{
			Gloves = ItemToEquip;
			bEquipped = true;
		}

		break;
	}
	case EEquipmentType::EQUIPMENT_BOOTS:
	{
		if (Boots)
		{
			ServerUnequipItem(Boots);
		}

		if (!Boots)
		{
			Boots = ItemToEquip;
			bEquipped = true;
		}

		break;
	}
	case EEquipmentType::EQUIPMENT_NECK:
	{
		if (Necklace)
		{
			ServerUnequipItem(Necklace);
		}

		if (!Necklace)
		{
			Necklace = ItemToEquip;
			bEquipped = true;
		}

		break;
	}
	case EEquipmentType::EQUIPMENT_EARRING:
	{
		if (PlacedLeft == -1)
		{
			if (!LeftEarring)
			{
				LeftEarring = ItemToEquip;
				bEquipped = true;
				break;
			}
			else if (!RightEarring)
			{
				RightEarring = ItemToEquip;
				bEquipped = true;
				break;
			}
			else if (LeftEarring->RequiredLevel <= RightEarring->RequiredLevel)
			{
				ServerUnequipItem(LeftEarring, true);

				if (!LeftEarring)
				{
					LeftEarring = ItemToEquip;
					bEquipped = true;
				}

				break;
			}
			else if (RightEarring->RequiredLevel < LeftEarring->RequiredLevel)
			{
				ServerUnequipItem(RightEarring);
				
				if (!RightEarring)
				{
					RightEarring = ItemToEquip;
					bEquipped = true;
				}

				break;
			}
		}
		else if (PlacedLeft == 0)
		{
			if (RightEarring)
			{
				ServerUnequipItem(RightEarring);
			}

			if (!RightEarring)
			{
				RightEarring = ItemToEquip;
				bEquipped = true;
			}

			break;

		}
		else if (PlacedLeft == 1)
		{
			if (LeftEarring)
			{
				ServerUnequipItem(LeftEarring, true);
			}

			if (!LeftEarring)
			{
				LeftEarring = ItemToEquip;
				bEquipped = true;
			}

			break;
		}
	}
	case EEquipmentType::EQUIPMENT_RING:
	{
		if (PlacedLeft == -1)
		{
			if (!LeftRing)
			{
				LeftRing = ItemToEquip;
				bEquipped = true;
				break;
			}
			else if (!RightRing)
			{
				RightRing = ItemToEquip;
				bEquipped = true;
				break;
			}
			else if (LeftRing->RequiredLevel <= RightRing->RequiredLevel)
			{
				ServerUnequipItem(LeftRing, true);

				if (!LeftRing)
				{
					LeftRing = ItemToEquip;
					bEquipped = true;
				}
				break;
			}
			else if (RightRing->RequiredLevel < LeftRing->RequiredLevel)
			{
				ServerUnequipItem(RightRing);

				if (!RightRing)
				{
					RightRing = ItemToEquip;
					bEquipped = true;
				}
				break;
			}
		}
		else if (PlacedLeft == 0)
		{
			if (RightRing)
			{
				ServerUnequipItem(RightRing);
			}

			if (!RightRing)
			{
				RightRing = ItemToEquip;
				bEquipped = true;
			}

			break;

		}
		else if (PlacedLeft == 1)
		{
			if (LeftRing)
			{
				ServerUnequipItem(LeftRing, true);
			}

			if (!LeftRing)
			{
				LeftRing = ItemToEquip;
				bEquipped = true;
			}

			break;
		}
	}
	default:
		break;
	}

	if (!bEquipped)
	{
		return;
	}

	MyInventory->RemoveItemFromInventory(ItemToEquip);

	for (auto& Attribute : ItemToEquip->BaseAttributes)
	{
		switch (Attribute.AffectedAttribute)
		{
		case EAttribute::ATTRIB_MAX_HEALTH:
			MaximumHealth += Attribute.Value;
			break;
		case EAttribute::ATTRIB_MAX_MANA:
			MaximumMana += Attribute.Value;
			break;
		case EAttribute::ATTRIB_ACC:
			Accuracy += Attribute.Value;
			break;
		case EAttribute::ATTRIB_MIN_PHYATK:
			MinimumPhysicalAttackPower += Attribute.Value;
			break;
		case EAttribute::ATTRIB_MAX_PHYATK:
			MaximumPhysicalAttackPower += Attribute.Value;
			break;
		case EAttribute::ATTRIB_MIN_MAGATK:
			MinimumMagicalAttackPower += Attribute.Value;
			break;
		case EAttribute::ATTRIB_MAX_MAGATK:
			MaximumMagicalAttackPower += Attribute.Value;
			break;
		case EAttribute::ATTRIB_PHYDEF:
			PhysicalDefense += Attribute.Value;
			break;
		case EAttribute::ATTRIB_MAGDEF:
			MagicalDefense += Attribute.Value;
			break;
		case EAttribute::ATTRIB_EVASION:
			Evasion += Attribute.Value;
			break;
		case EAttribute::ATTRIB_DMGREDUCE:
			DamageReduction += Attribute.Value;
			break;
		case EAttribute::ATTRIB_LIFESTEAL:
			LifeSteal += Attribute.Value;
			break;
		case EAttribute::ATTRIB_CRITRATE:
			CriticalRate += Attribute.Value;
			break;
		case EAttribute::ATTRIB_CRITDMG:
			CriticalDamage += Attribute.Value;
			break;
		case EAttribute::ATTRIB_LUCK:
			Luck += Attribute.Value;
			break;
		case EAttribute::ATTRIB_MOVESPD:
		{
			MovementSpeed += Attribute.Value;
			GetCharacterMovement()->MaxWalkSpeed += Attribute.Value;
			break;
		}
		default:
			break;
		}
	}

	if (ItemToEquip->CurrentEnhancementLevel > 0)
	{
		for (auto& EnhancedAttribute : ItemToEquip->CalculateBonusStatsBasedOnEnhancementLevel())
		{
			switch (EnhancedAttribute.AffectedAttribute)
			{
			case EAttribute::ATTRIB_MAX_HEALTH:
				MaximumHealth += EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_MAX_MANA:
				MaximumMana += EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_ACC:
				Accuracy += EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_MIN_PHYATK:
				MinimumPhysicalAttackPower += EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_MAX_PHYATK:
				MaximumPhysicalAttackPower += EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_MIN_MAGATK:
				MinimumMagicalAttackPower += EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_MAX_MAGATK:
				MaximumMagicalAttackPower += EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_PHYDEF:
				PhysicalDefense += EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_MAGDEF:
				MagicalDefense += EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_EVASION:
				Evasion += EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_DMGREDUCE:
				DamageReduction += EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_LIFESTEAL:
				LifeSteal += EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_CRITRATE:
				CriticalRate += EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_CRITDMG:
				CriticalDamage += EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_LUCK:
				Luck += EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_MOVESPD:
			{
				MovementSpeed += EnhancedAttribute.Value;
				GetCharacterMovement()->MaxWalkSpeed += EnhancedAttribute.Value;
				
				break;
			}
			default:
				break;
			}
		}
	}

	//-2 indicates an item is in an equipment slot
	ItemToEquip->CurrentSlotIndex = -2;
	ItemToEquip->ClientItemMoved();

	ServerRecalculateBuffs();
}


bool ATheEternalTowerCharacter::ServerEquipItem_Validate(AEquippableItem* ItemToEquip, int32 PlacedLeft)
{
	return true;
}


void ATheEternalTowerCharacter::ServerUnequipItem_Implementation(AEquippableItem* ItemToUnequip, bool UnequipLeft, int32 SlotToUnequipInto)
{
	if (!ItemToUnequip)
	{
		return;
	}

	if (MyInventory->UsedSlotCount == MyInventory->NumberOfSlots)
	{
		return;
	}

	for (auto& Attribute : ItemToUnequip->BaseAttributes)
	{
		switch (Attribute.AffectedAttribute)
		{
		case EAttribute::ATTRIB_MAX_HEALTH:
			MaximumHealth -= Attribute.Value;
			if (Health > MaximumHealth)
				Health = MaximumHealth;
			break;
		case EAttribute::ATTRIB_MAX_MANA:
			MaximumMana -= Attribute.Value;
			if (Mana > MaximumMana)
				Mana = MaximumMana;
			break;
		case EAttribute::ATTRIB_ACC:
			Accuracy -= Attribute.Value;
			break;
		case EAttribute::ATTRIB_MIN_PHYATK:
			MinimumPhysicalAttackPower -= Attribute.Value;
			break;
		case EAttribute::ATTRIB_MAX_PHYATK:
			MaximumPhysicalAttackPower -= Attribute.Value;
			break;
		case EAttribute::ATTRIB_MIN_MAGATK:
			MinimumMagicalAttackPower -= Attribute.Value;
			break;
		case EAttribute::ATTRIB_MAX_MAGATK:
			MaximumMagicalAttackPower -= Attribute.Value;
			break;
		case EAttribute::ATTRIB_PHYDEF:
			PhysicalDefense -= Attribute.Value;
			break;
		case EAttribute::ATTRIB_MAGDEF:
			MagicalDefense -= Attribute.Value;
			break;
		case EAttribute::ATTRIB_EVASION:
			Evasion -= Attribute.Value;
			break;
		case EAttribute::ATTRIB_DMGREDUCE:
			DamageReduction -= Attribute.Value;
			break;
		case EAttribute::ATTRIB_LIFESTEAL:
			LifeSteal -= Attribute.Value;
			break;
		case EAttribute::ATTRIB_CRITRATE:
			CriticalRate -= Attribute.Value;
			break;
		case EAttribute::ATTRIB_CRITDMG:
			CriticalDamage -= Attribute.Value;
			break;
		case EAttribute::ATTRIB_LUCK:
			Luck -= Attribute.Value;
			break;
		case EAttribute::ATTRIB_MOVESPD:
		{
			MovementSpeed -= Attribute.Value;
			GetCharacterMovement()->MaxWalkSpeed -= Attribute.Value;
			
			break;
		}
		default:
			break;
		}
	}

	if (ItemToUnequip->CurrentEnhancementLevel > 0)
	{
		for (auto& EnhancedAttribute : ItemToUnequip->CalculateBonusStatsBasedOnEnhancementLevel())
		{
			switch (EnhancedAttribute.AffectedAttribute) 
			{
			case EAttribute::ATTRIB_MAX_HEALTH:
				MaximumHealth -= EnhancedAttribute.Value;
				if (Health > MaximumHealth)
					Health = MaximumHealth;
				break;
			case EAttribute::ATTRIB_MAX_MANA:
				MaximumMana -= EnhancedAttribute.Value;
				if (Mana > MaximumMana)
					Mana = MaximumMana;
				break;
			case EAttribute::ATTRIB_ACC:
				Accuracy -= EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_MIN_PHYATK:
				MinimumPhysicalAttackPower -= EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_MAX_PHYATK:
				MaximumPhysicalAttackPower -= EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_MIN_MAGATK:
				MinimumMagicalAttackPower -= EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_MAX_MAGATK:
				MaximumMagicalAttackPower -= EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_PHYDEF:
				PhysicalDefense -= EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_MAGDEF:
				MagicalDefense -= EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_EVASION:
				Evasion -= EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_DMGREDUCE:
				DamageReduction -= EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_LIFESTEAL:
				LifeSteal -= EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_CRITRATE:
				CriticalRate -= EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_CRITDMG:
				CriticalDamage -= EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_LUCK:
				Luck -= EnhancedAttribute.Value;
				break;
			case EAttribute::ATTRIB_MOVESPD:
			{
				MovementSpeed -= EnhancedAttribute.Value;
				GetCharacterMovement()->MaxWalkSpeed -= EnhancedAttribute.Value;
				
				break;
			}
			default:
				break;
			}
		}
	}

	switch (ItemToUnequip->EquipmentType)
	{
	case EEquipmentType::EQUIPMENT_HELMET:
	{
		Helmet = NULL;
		break;
	}
	case EEquipmentType::EQUIPMENT_ARMOR:
	{
		Armor = NULL;
		
		if(BaseSkeletalMesh)
			MulticastEquipArmor(this, BaseSkeletalMesh);
		
		break;
	}
	case EEquipmentType::EQUIPMENT_BOOTS:
	{
		Boots = NULL;
		break;
	}
	case EEquipmentType::EQUIPMENT_GLOVES:
	{
		Gloves = NULL;
		break;
	}
	case EEquipmentType::EQUIPMENT_NECK:
	{
		Necklace = NULL;
		break;
	}
	case EEquipmentType::EQUIPMENT_WEAPON:
	{
		Weapon = NULL;
		MulticastEquipWeapon(this, ItemToUnequip);
		break;
	}
	case EEquipmentType::EQUIPMENT_EARRING:
	{
		if (UnequipLeft)
			LeftEarring = NULL;
		else
			RightEarring = NULL;
		break;
	}
	case EEquipmentType::EQUIPMENT_RING:
	{
		if (UnequipLeft)
			LeftRing = NULL;
		else
			RightRing = NULL;
		break;
	}
	default:
		break;
	}

	MyInventory->AddItemToInventory(ItemToUnequip, SlotToUnequipInto);
	ItemToUnequip = NULL;

	ServerRecalculateBuffs();
	
	return;
}

bool ATheEternalTowerCharacter::ServerUnequipItem_Validate(AEquippableItem* ItemToUnequip, bool UnequipLeft, int32 SlotToUnequipInto)
{
	return true;
}


void ATheEternalTowerCharacter::BeginPlay()
{
	// Call the base class
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(DebugTimerHandle, this, &ATheEternalTowerCharacter::DebugFunction2, 0.1f, true);

	while (!MyInventory)
	{
		TArray<UInventory*> InventoryComponents;
		GetComponents<UInventory>(InventoryComponents);

		MyInventory = InventoryComponents[0];
	}

	MyInventory->Money = FCurrency(24, 98, 200) * 4;
}


void ATheEternalTowerCharacter::MulticastEquipArmor_Implementation(ATheEternalTowerCharacter* Character, USkeletalMesh* NewArmoredMesh)
{
	if (Character && NewArmoredMesh)
	{
		Character->GetMesh()->SetSkeletalMesh(NewArmoredMesh);
	}
}


void ATheEternalTowerCharacter::MulticastEquipWeapon_Implementation(ATheEternalTowerCharacter* Character, AEquippableItem* OldWeapon, AEquippableItem* NewWeapon)
{
	if (Character)
	{
		if (OldWeapon)
		{
			OldWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			OldWeapon->SetActorHiddenInGame(true);
		}
		
		if (NewWeapon)
		{

			FName SocketName = TEXT("WeaponSocket");
			NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
			NewWeapon->SetActorHiddenInGame(false);
		}
	}
}