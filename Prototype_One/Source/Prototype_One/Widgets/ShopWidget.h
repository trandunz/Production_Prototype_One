#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopWidget.generated.h"

UCLASS()
class PROTOTYPE_ONE_API UShopWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void SellAnyItems();

	UFUNCTION()
	void SellFoodItems();

	UFUNCTION()
	void SellCrownItems();

	UFUNCTION()
	void SellMaskItems();
	
	UFUNCTION()
	void SellAntlerItems();

	UFUNCTION()
	void Back();

	UFUNCTION()
	void OnUpgradeHealth();

	UFUNCTION()
	void OnUpgradeStamina();

	UFUNCTION()
	void OnUpgradeCarryWeight();

	UFUNCTION()
	void OnBuyStaminaPotion();
	UFUNCTION()
	void OnBuyHealthPotion();

	UFUNCTION()
	void OnBuyGateTicket();
	UFUNCTION()
	void OnEndGame();
public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* Sell;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* SellMeat;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* SellAntler;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* SellMask;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* SellCrown;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* GoBack;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* Money;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* UpgradeHealth;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* UpgradeStamina;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* UpgradeCarryWeight;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* HealthIcon;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* StaminaIcon;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* CarryWeightIcon;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* BuyHealthPotion;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* BuyStaminaPotion;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* BuyGateTicket;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* EndGame;
	
	UPROPERTY(EditAnywhere)
	float GateTicketPrice{10000};
private:
	class ABag* Bag;
};
