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

