// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

// enum for potion type
enum class EPotionType : uint8
{
	Health UMETA(DisplayName = "Health"),
	Mana UMETA(DisplayName = "Mana"),
};

class APrototype_OneCharacter;

class PROTOTYPE_ONE_API Potion
{
public:
	int32 amountRestored;
	EPotionType potionType;
	Potion();
	/**
	 * @brief pass in a pointer to the value you want to increase
	 * @param _value 
	 */
	void Consume(int *_value);
	~Potion();
};
