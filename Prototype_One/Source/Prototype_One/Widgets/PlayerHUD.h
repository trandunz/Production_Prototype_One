#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

UCLASS()
class PROTOTYPE_ONE_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void UpdateInteractionText(FString _interactionKey = "", FString _message = "");
	void UpdateHealthText(int _currentHealth);
	void UpdateMoneyText(int _currentMoney);
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* InteractionText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UDialogueWidget* DialogueWidget;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* HealthText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* MoneyText;
};
