#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPGEntityComponent.generated.h"

USTRUCT()
struct FEntityProperties
{
	GENERATED_BODY()

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
	float DashStaminaCost{10.0f};
	UPROPERTY(EditAnywhere)
	float AttackStaminaCost{20.0f};
	UPROPERTY(VisibleAnywhere)
	float CurrentStaminaTime{};
	UPROPERTY(VisibleAnywhere)
	bool IsStaminaDraining{false};
	
	// Currency
	UPROPERTY(EditAnywhere)
	int CurrentMoney{200};

	UPROPERTY(EditAnywhere)
	int UpgradeCost{100};
	
	UPROPERTY(VisibleAnywhere)
	int HealthCurrentLevel{1};
	UPROPERTY(VisibleAnywhere)
	int StaminaCurrentLevel{1};
	UPROPERTY(VisibleAnywhere)
	int CarryWightCurrentLevel{1};

	UPROPERTY(VisibleAnywhere)
	float UpgradeAmount{20.0f};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOTYPE_ONE_API URPGEntityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URPGEntityComponent();

protected:
	virtual void BeginPlay() override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void TakeDamage(int _amount);
	void Heal(int _amount);

	void UpgradeHealth();
	void UpgradeStamina();
	void UpgradeCarryWeight();

	UPROPERTY(EditAnywhere)
	FEntityProperties Properties{};
private:

	void StaminaRegenDrain(float dt);
};
