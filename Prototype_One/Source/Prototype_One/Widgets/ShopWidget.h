#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopWidget.generated.h"

class UWidget_WinMenu;
UCLASS()
class PROTOTYPE_ONE_API UShopWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void SellAnyItems();

	UFUNCTION()
	void Back();

	UFUNCTION()
	void OnUpgradeHealth();

	UFUNCTION()
	void OnUpgradeStamina();

	UFUNCTION()
	void OnUpgradeCarryWeight();

	UFUNCTION()
	void OnUpgradeAttackDamage();

	//UFUNCTION()
	//void OnBuyStaminaPotion();
	UFUNCTION()
	void OnBuyHealthPotion();

	UFUNCTION()
	void OnBuyGateTicket();
public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* HealthCost;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* StaminaCost;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* CarryWeightCost;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* AttackCost;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* HealthLevel;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* StaminaLevel;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* CarryWeightLevel;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* AttackLevel;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* Sell;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* GoBack;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* UpgradeHealth;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* UpgradeStamina;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* UpgradeCarryWeight;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* UpgradeAttackDamage;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* BuyHealthPotion;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* BuyGateTicket;
	
	
	UPROPERTY(EditAnywhere)
	float GateTicketPrice{10000};
private:
	class ABag* Bag;

	int32 PotionCost = 50;
};
