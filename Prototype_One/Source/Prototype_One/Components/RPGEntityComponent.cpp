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

	//if (auto* gamemode = Cast<APrototype_OneGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	//{
	//	if (gamemode->SavedPlayerData)
	//	{
	//		gamemode->SavedPlayerData->SavedProperties = Properties;
	//	}
	//	gamemode->SaveGame();
	//}
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
		Properties.MaxHealth += Properties.UpgradeAmount;
		Properties.CurrentMoney -= Properties.UpgradeCost * Properties.HealthCurrentLevel;
		Properties.HealthCurrentLevel++;
	}
}

void URPGEntityComponent::UpgradeStamina()
{
	if (Properties.CurrentMoney >= Properties.UpgradeCost * Properties.StaminaCurrentLevel)
	{
		Properties.MaxStamina += Properties.UpgradeAmount;
		Properties.UpgradeAmount += 20.0;
		Properties.CurrentMoney -= Properties.UpgradeCost * Properties.StaminaCurrentLevel;
		Properties.StaminaCurrentLevel++;
	}
}

void URPGEntityComponent::UpgradeCarryWeight()
{
	if (Properties.CurrentMoney >= Properties.UpgradeCost * Properties.CarryWeightCurrentLevel)
	{
		Properties.MaxCarryWeight += Properties.UpgradeAmount;
		Properties.CurrentMoney -= Properties.UpgradeCost * Properties.CarryWeightCurrentLevel;
		Properties.CarryWeightCurrentLevel++;
	}
}

void URPGEntityComponent::UpgradeAttackDamage()
{
	if (Properties.CurrentMoney >= Properties.UpgradeCost * Properties.AttackDamageLevel)
	{
		Properties.MaxCarryWeight += Properties.UpgradeAmount;
		Properties.CurrentMoney -= Properties.UpgradeCost * Properties.AttackDamageLevel;
		Properties.AttackDamageLevel++;
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
				UE_LOG(LogTemp, Log, TEXT("Stamina Time : %s"), *FString::FromInt(Properties.CurrentStaminaTime));
				Properties.CurrentStaminaTime -= dt;
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Regen Stamina: Smaina = %s"), *FString::FromInt(Properties.CurrentStamina));
				Properties.CurrentStaminaTime = Properties.MaxStaminaRegenTime; // Reset stamina timer
				Properties.CurrentStamina += Properties.StaminaRegen * Properties.StaminaCurrentLevel;
			}
		}
	}
}

