#include "RPGEntityComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Prototype_One/Prototype_OneGameMode.h"
#include "Prototype_One/SavedPlayerData.h"

URPGEntityComponent::URPGEntityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void URPGEntityComponent::BeginPlay()
{
	Super::BeginPlay();
	Properties.CurrentHealth = Properties.MaxHealth;
	Properties.CurrentStamina = Properties.MaxStamina;
}

void URPGEntityComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);

	if (auto* gamemode = Cast<APrototype_OneGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		if (gamemode->SavedPlayerData)
		{
			gamemode->SavedPlayerData->SavedProperties = Properties;
		}
		gamemode->SaveGame();
	}
}

void URPGEntityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	StaminaRegenDrain(DeltaTime);
}

void URPGEntityComponent::TakeDamage(int _amount)
{
	if (Properties.CurrentHealth > 0)
	{
		Properties.CurrentHealth -= _amount;

		if (Properties.CurrentHealth <= 0)
		{
			Properties.CurrentHealth = 0;
		}
	}
}

void URPGEntityComponent::Heal(int _amount)
{
	if (Properties.CurrentHealth > 0)
	{
		Properties.CurrentHealth += _amount;

		if (Properties.CurrentHealth >= Properties.MaxHealth)
		{
			Properties.CurrentHealth = Properties.MaxHealth;
		}
	}
}

void URPGEntityComponent::UpgradeHealth()
{
	if (Properties.CurrentMoney >= Properties.UpgradeCost * Properties.HealthCurrentLevel)
	{
		Properties.CurrentHealth += Properties.UpgradeAmount;
		Properties.CurrentMoney -= Properties.UpgradeCost * Properties.HealthCurrentLevel;
		Properties.HealthCurrentLevel++;
	}
}

void URPGEntityComponent::UpgradeStamina()
{
	if (Properties.CurrentMoney >= Properties.UpgradeCost * Properties.StaminaCurrentLevel)
	{
		Properties.CurrentStamina += Properties.UpgradeAmount;
		Properties.UpgradeAmount += 20.0;
		Properties.CurrentMoney -= Properties.UpgradeCost * Properties.StaminaCurrentLevel;
		Properties.StaminaCurrentLevel++;
	}
}

void URPGEntityComponent::UpgradeCarryWeight()
{
	if (Properties.CurrentMoney >= Properties.UpgradeCost * Properties.CarryWightCurrentLevel)
	{
		Properties.CurrentCarryWeight += Properties.UpgradeAmount;
		Properties.CurrentMoney -= Properties.UpgradeCost * Properties.CarryWightCurrentLevel;
		Properties.CarryWightCurrentLevel++;
	}
}

void URPGEntityComponent::StaminaRegenDrain(float dt)
{
	if (Properties.IsStaminaDraining == true && Properties.CurrentStamina > 0) // Stamina drain
	{
		if (Properties.CurrentStaminaTime > 0)
		{
			Properties.CurrentStaminaTime -= dt;
		}
		else
		{
			Properties.CurrentStaminaTime = Properties.MaxStaminaDrainTime; // Reset stamina timer
			Properties.CurrentStamina -= Properties.StaminaDrain;
		}
	}
	else
	{
		if (Properties.CurrentStamina < Properties.MaxStamina)
		{
			if (Properties.CurrentStaminaTime > 0)
			{
				Properties.CurrentStaminaTime -= dt;
			}
			else
			{
				Properties.CurrentStaminaTime = Properties.MaxStaminaRegenTime; // Reset stamina timer
				Properties.CurrentStamina += Properties.StaminaRegen;
			}
		}
	}
}

