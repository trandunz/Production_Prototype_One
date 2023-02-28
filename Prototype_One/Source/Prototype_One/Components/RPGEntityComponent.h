#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPGEntityComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOTYPE_ONE_API URPGEntityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URPGEntityComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void TakeDamage(int _amount);
	void Heal(int _amount);

	void UpgradeHealth();
	void UpgradeStamina();
	void UpgradeCarryWeight();
	
	// Health
	UPROPERTY(VisibleAnywhere)
	int CurrentHealth{};
	UPROPERTY(EditAnywhere)
	int MaxHealth{100};

	// Mana
	UPROPERTY(VisibleAnywhere)
	int CurrentCarryWeight{};
	UPROPERTY(EditAnywhere)
	int MaxCarryWeight{100};

	// Stamina
	UPROPERTY(VisibleAnywhere)
	int CurrentStamina{};
	UPROPERTY(EditAnywhere)
	int MaxStamina{100};
	UPROPERTY(EditAnywhere)
	int StaminaRegen{1};
	UPROPERTY(EditAnywhere)
	int StaminaDrain{1};
	UPROPERTY(EditAnywhere)
	float MaxStaminaDrainTime{1};
	UPROPERTY(EditAnywhere)
	float MaxStaminaRegenTime{1};
	UPROPERTY(EditAnywhere)
	float MinimumStaminaToSprint{10.0f};
	UPROPERTY(EditAnywhere)
	float StaminaDamageDodge{10.0f};
	UPROPERTY(EditAnywhere)
	float StaminaDamageAttack{20.0f};
	UPROPERTY(VisibleAnywhere)
	float CurrentStaminaTime{};
	UPROPERTY(VisibleAnywhere)
	bool IsStaminaDraining{false};
	
	// Currency
	UPROPERTY(EditAnywhere)
	int CurrentMoney{};

	UPROPERTY(EditAnywhere)
	int UpgradeCost{100};
	
	UPROPERTY(VisibleAnywhere)
	int HealthCurrentLevel{0};
	UPROPERTY(VisibleAnywhere)
	int StaminaCurrentLevel{0};
	UPROPERTY(VisibleAnywhere)
	int CarryWightCurrentLevel{0};

	UPROPERTY(VisibleAnywhere)
	float UpgradeAmount{20.0f};
private:

	void StaminaRegenDrain(float dt);
};
