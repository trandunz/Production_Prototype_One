#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"


UCLASS()
class PROTOTYPE_ONE_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

	UFUNCTION()
	void ResumeButtonPress();
	UFUNCTION()
	void QuitButtonPress();

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* ResumeButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* QuitButton;
};
