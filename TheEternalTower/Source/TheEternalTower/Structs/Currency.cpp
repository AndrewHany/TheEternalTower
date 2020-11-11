// Fill out your copyright notice in the Description page of Project Settings.

#include "TheEternalTower.h"
#include "Currency.h"
#include "Engine.h"


FCurrency::FCurrency()
{
	Gold	= 0;
	Silver	= 0;
	Bronze	= 0;
}


FCurrency::FCurrency(int32 _Gold, uint8 _Silver, uint8 _Bronze)
{
	Gold	= _Gold;
	Silver	= _Silver;
	Bronze	= _Bronze;
}


int32 FCurrency::ConvertToBronze(FCurrency CurrencyMoney)
{
	return CurrencyMoney.Bronze + CurrencyMoney.Silver * 100 + CurrencyMoney.Gold * 10000;
}


FCurrency FCurrency::ConvertFromBronze(int32 TotalBronze)
{
	FCurrency CurrencyMoney;

	CurrencyMoney.Gold   = TotalBronze / 10000;
	CurrencyMoney.Silver = (TotalBronze % 10000) / 100;
	CurrencyMoney.Bronze = (TotalBronze % 10000) % 100;

	return CurrencyMoney;
}



FCurrency& FCurrency::operator=(FCurrency RHS)
{
	this->Gold	 = RHS.Gold;
	this->Silver = RHS.Silver;
	this->Bronze = RHS.Bronze;

	return *this;
}


FCurrency& FCurrency::operator+=(FCurrency RHS)
{
	int32 TotalAmountInBronze = ConvertToBronze(*this) + ConvertToBronze(RHS);

	*this = ConvertFromBronze(TotalAmountInBronze);

	return *this;
}


FCurrency& FCurrency::operator-=(FCurrency RHS)
{
	int32 TotalAmountInBronze = ConvertToBronze(*this) - ConvertToBronze(RHS);

	*this = ConvertFromBronze(TotalAmountInBronze);

	return *this;
}


FCurrency& FCurrency::operator*=(int32 Number)
{
	int32 TotalAmountInBronze = ConvertToBronze(*this) * Number;

	*this = ConvertFromBronze(TotalAmountInBronze);

	return *this;
}


FCurrency& FCurrency::operator/=(int32 Number)
{
	int32 TotalAmountInBronze = ConvertToBronze(*this) / Number;

	*this = ConvertFromBronze(TotalAmountInBronze);

	return *this;
}