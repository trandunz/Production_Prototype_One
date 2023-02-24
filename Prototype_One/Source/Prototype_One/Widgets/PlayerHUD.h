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
	void UpdateHealth(int _health, int _maxHealth);
	void UpdateMana(int _mana, int _maxMana);
	void UpdateStamina(int _stamina, int _maxStamina);
	void UpdateInteractionText(FString _interactionKey = "", FString _message = "");
	void UpdateSneakStatus(bool _seen);
	void ToggleDebugMenu();
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* InteractionText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UDialogueWidget* DialogueWidget;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UDebugMenu* DebugMenu;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UProgressBar* HealthBar;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UProgressBar* ManaBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UProgressBar* StaminaBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* Crosshair;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* Crosshair_Overlay;

	UPROPERTY(EditAnywhere)
	class UTexture2D* SeenImage;
	UPROPERTY(EditAnywhere)
	class UTexture2D* HiddenImage;
};
