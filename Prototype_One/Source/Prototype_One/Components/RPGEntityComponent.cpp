#include "RPGEntityComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Prototype_One/Gamemodes/Prototype_OneGameMode.h"
#include "Prototype_One/Bag.h"
#include "Prototype_One/SavedPlayerData.h"

URPGEntityComponent::URPGEntityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	Properties.StaminaDelayTimer = 0.0f;
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

	if (Properties.StaminaDelayTimer > 0)
	{
		Properties.StaminaDelayTimer -= DeltaTime;
	}
	
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
	Properties.MaxHealth += Properties.UpgradeAmount;
	Properties.HealthCurrentLevel++;	
}

void URPGEntityComponent::UpgradeStamina()
{
	Properties.MaxStamina += Properties.UpgradeAmount;
	Properties.UpgradeAmount += 20.0;
	Properties.StaminaCurrentLevel++;
	
	Properties.StaminaRegen ++; // Increase stamina regen
}

void URPGEntityComponent::UpgradeCarryWeight()
{
	//Properties.MaxCarryWeight += Properties.UpgradeAmount;
	Properties.CarryWeightCurrentLevel++;

	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABag::StaticClass(), actors);
	for(auto bagActor : actors)
	{
		if (auto* bag = Cast<ABag>(bagActor))
		{
			bag->PlayerWeightThreshold += 10;
			bag->StoppingThreshold += 10;
		}
	}
}

void URPGEntityComponent::UpgradeAttackDamage()
{
	Properties.MaxCarryWeight += Properties.UpgradeAmount;
	Properties.AttackDamageLevel++;	
}

void URPGEntityComponent::StaminaRegenDrain(float dt)
{
	if (Properties.IsStaminaDraining)  // Draining
	{
		if (Properties.CurrentStaminaTime > 0)// Stamina drain
		{
			Properties.CurrentStaminaTime -= dt;
			if (Properties.CurrentStaminaTime < 0.0f)
			{
				Properties.CurrentStaminaTime = 0.0f;
			}
		}
		else
		{
			Properties.CurrentStaminaTime = Properties.MaxStaminaDrainTime; // Reset stamina timer
			Properties.CurrentStamina -= Properties.StaminaDrain;
		}
	}
	else // Regen
	{
		if (Properties.CurrentStamina < Properties.MaxStamina)
		{
			if (Properties.CurrentStaminaTime > 0)
			{
				Properties.CurrentStaminaTime -= dt;
			}
			else if (Properties.StaminaDelayTimer <= 0)
			{
				Properties.CurrentStaminaTime = Properties.MaxStaminaRegenTime; // Reset stamina timer
				Properties.CurrentStamina += Properties.StaminaRegen;
			}
		}
	}
}

