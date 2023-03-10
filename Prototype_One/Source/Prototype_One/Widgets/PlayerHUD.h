#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

class Fade;
UCLASS()
class PROTOTYPE_ONE_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
	Fade* fade = nullptr;
	float fadeDelay = 1.0f;
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void UpdateHealth(int _health, int _maxHealth);
	void UpdateMana(int _mana, int _maxMana);
	void UpdateStamina(int _stamina, int _maxStamina);
	void UpdateInteractionText(FString _interactionKey = "", FString _message = "");
	void UpdateSneakStatus(int _seen);
	void ToggleDebugMenu();
	void FadeIn();
	void FadeOut();

	void ChangeFadeTime(float _fadeTime);
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* InteractionText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UDialogueWidget* DialogueWidget;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UDebugMenu* DebugMenu;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UProgressBar* EncumberanceMeter;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UProgressBar* ManaBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UProgressBar* StaminaBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UPauseMenuWidget* PauseMenu;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* Crosshair;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* Crosshair_Overlay;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* BlackFade;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* BagIcon;

	UPROPERTY(EditAnywhere)
	class UTexture2D* BagOpenImage;

	UPROPERTY(EditAnywhere)
	class UTexture2D* BagClosedImage;
	
	UPROPERTY(EditAnywhere)
	class UTexture2D* SeenImage;
	UPROPERTY(EditAnywhere)
	class UTexture2D* HiddenImage;
	UPROPERTY(EditAnywhere)
	class UTexture2D* HalfHiddenImage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (BindWidget))
	class UInventoryWidget* InventoryWidget;
};
