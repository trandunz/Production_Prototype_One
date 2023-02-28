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
	void Back();

	UFUNCTION()
	void OnUpgradeHealth();

	UFUNCTION()
	void OnUpgradeStamina();

	UFUNCTION()
	void OnUpgradeCarryWeight();
public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* Sell;

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
	
	
};
