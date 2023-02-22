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
}

void URPGEntityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
