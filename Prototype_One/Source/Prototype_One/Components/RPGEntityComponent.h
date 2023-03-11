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
	UPROPERTY(VisibleAnywhere)
	int HealthCurrentLevel{1};

	// Carry weight (get from PlayerInventory)
	//UPROPERTY(VisibleAnywhere)
	//int CurrentCarryWeight{};
	UPROPERTY(EditAnywhere)
	int MaxCarryWeight{100};
	UPROPERTY(VisibleAnywhere)
	int CarryWeightCurrentLevel{1};
	UPROPERTY(VisibleAnywhere)
	int AttackDamageLevel{1};

	// Stamina
	UPROPERTY(VisibleAnywhere)
	int CurrentStamina{};
	UPROPERTY(EditAnywhere)
	int MaxStamina{100};
	UPROPERTY(VisibleAnywhere)
	int StaminaCurrentLevel{1};
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
	
	// Currency (get from PlayerInventory)
	//UPROPERTY(EditAnywhere)
	//int CurrentMoney{};

	// Upgrading
	UPROPERTY(EditAnywhere)
	int UpgradeCost{100};
	UPROPERTY(VisibleAnywhere)
	float UpgradeAmount{20.0f};

	UPROPERTY(VisibleAnywhere)
	int AttackDamage{50};
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
	void UpgradeAttackDamage();

	UPROPERTY(EditAnywhere)
	FEntityProperties Properties{};

private:

	void StaminaRegenDrain(float dt);
};
