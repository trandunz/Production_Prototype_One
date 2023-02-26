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
public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* Sell;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* GoBack;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* Money;
	
};
