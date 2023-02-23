#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DebugMenu.generated.h"


UCLASS()
class PROTOTYPE_ONE_API UDebugMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void OnCloseMenu();
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* Background;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* CloseButton;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* PlayerHealthText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* PlayerMoneyText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* PlayerManaText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* PlayerStaminaText;
};
