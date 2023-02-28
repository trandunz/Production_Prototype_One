// Fill out your copyright notice in the Description page of Project Settings.


#include "Potion.h"

Potion::Potion()
{
}

void Potion::Consume(int *_value)
{
	*_value += amountRestored;
}

Potion::~Potion()
{
}
