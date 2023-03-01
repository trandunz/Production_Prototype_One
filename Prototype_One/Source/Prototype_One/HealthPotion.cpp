// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPotion.h"
#include "Prototype_OneCharacter.generated.h"
#include "GameplayStatics.generated.h"

HealthPotion::HealthPotion()
{
	potionType = EPotionType::Health;
	amountRestored = 33;
}

HealthPotion::~HealthPotion()
{
}
