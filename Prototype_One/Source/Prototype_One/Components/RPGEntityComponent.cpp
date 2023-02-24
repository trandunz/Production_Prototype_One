#include "RPGEntityComponent.h"

URPGEntityComponent::URPGEntityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URPGEntityComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	CurrentMana = MaxMana;
	CurrentStamina = MaxStamina;
	CurrentMoney = 69; // Nice
}

void URPGEntityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	StaminaRegenAndDrain(DeltaTime);
}

void URPGEntityComponent::TakeDamage(int _amount)
{
	if (CurrentHealth > 0)
	{
		CurrentHealth -= _amount;

		if (CurrentHealth <= 0)
		{
			CurrentHealth = 0;
		}
	}
}

void URPGEntityComponent::Heal(int _amount)
{
	if (CurrentHealth > 0)
	{
		CurrentHealth += _amount;

		if (CurrentHealth >= MaxHealth)
		{
			CurrentHealth = MaxHealth;
		}
	}
}

void URPGEntityComponent::StaminaRegenAndDrain(float dt)
{
	if (IsStaminaDraining == true && CurrentStamina > 0) // Stamina drain
	{
		if (CurrentStaminaTime > 0)
		{
			CurrentStaminaTime -= dt;
		}
		else
		{
			CurrentStaminaTime = MaxStaminaDrainTime; // Reset stamina timer
			CurrentStamina -= StaminaDrain;
		}
	}
	else // Stamina Regen
	{
		if (CurrentStamina < MaxStamina)
		{
			if (CurrentStaminaTime > 0)
			{
				CurrentStaminaTime -= dt;
			}
			else
			{
				CurrentStaminaTime = MaxStaminaRegenTime; // Reset stamina timer
				CurrentStamina += StaminaRegen;
			}
		}
	}
}

