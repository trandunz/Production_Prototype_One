#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BP_MainMenuWidget.generated.h"

UCLASS()
class PROTOTYPE_ONE_API UBP_MainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void PlayGame();
	UFUNCTION()
	void OpenOptionsMenu();
	UFUNCTION()
	void QuitGame();
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* PlayButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* OptionsButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* QuitButton;
};
