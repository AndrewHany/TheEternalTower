// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "Currency.generated.h"

/** Structure that defines the game's used currency */
USTRUCT(BlueprintType)
struct FCurrency
{
	GENERATED_USTRUCT_BODY()

// PROPERTIES
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Currency)
		int32 Gold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Currency, meta = (ClampMin = "0", ClampMax = "99", UIMin = "0", UIMax = "99"))
		uint8 Silver;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Currency, meta = (ClampMin = "0", ClampMax = "99", UIMin = "0", UIMax = "99"))
		uint8 Bronze;


// FUNCTIONS
public:
	
	FCurrency();

	FCurrency(int32 _Gold, uint8 _Silver, uint8 _Bronze);
	
	//conversion functions for easier arithmetics

	static int32 ConvertToBronze(FCurrency CurrencyMoney);
	static FCurrency ConvertFromBronze(int32 TotalBronze);

	//copy/move constructor
	FCurrency& operator=(FCurrency rhs);

	
	//arithmetic operators
	
	FCurrency& operator+=(FCurrency rhs);

	friend FCurrency operator+(FCurrency LHS, const FCurrency& RHS)
	{
		LHS += RHS;
		return LHS;
	}

	
	FCurrency& operator-=(FCurrency rhs);

	friend FCurrency operator-(FCurrency LHS, const FCurrency& RHS)
	{
		LHS -= RHS;
		return LHS;
	}

	
	FCurrency& operator*=(int32 Number);

	friend FCurrency operator*(FCurrency LHS, int32 RHS)
	{
		LHS *= RHS;
		return LHS;
	}

	
	FCurrency& operator/=(int32 Number);

	friend FCurrency operator/(FCurrency LHS, int32 RHS)
	{
		LHS /= RHS;
		return LHS;
	}


	//relational operators

	friend bool operator<(const FCurrency& LHS, const FCurrency& RHS)
	{
		return FCurrency::ConvertToBronze(LHS) < FCurrency::ConvertToBronze(RHS);
	}

	friend bool operator>(const FCurrency& LHS, const FCurrency& RHS)
	{
		return RHS < LHS;
	}

	friend bool operator<=(const FCurrency& LHS, const FCurrency& RHS)
	{
		return !(LHS > RHS);
	}

	friend bool operator>=(const FCurrency& LHS, const FCurrency& RHS)
	{
		return !(LHS < RHS);
	}

	friend bool operator==(const FCurrency& LHS, const FCurrency& RHS)
	{
		return FCurrency::ConvertToBronze(LHS) == FCurrency::ConvertToBronze(RHS);
	}

	friend bool operator!=(const FCurrency& LHS, const FCurrency& RHS)
	{
		return !(LHS == RHS);
	}
};
