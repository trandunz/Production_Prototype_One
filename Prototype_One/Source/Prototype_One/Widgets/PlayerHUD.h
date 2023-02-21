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
	void UpdateInteractionText(FString _interactionKey = "", FString _message = "");
	void ShowDialogueBox(bool);
	UFUNCTION()
	void OnNextDialogueLine();
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* InteractionText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* DialogueBackground;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* DialogueText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* Dialogue_NextLine;
};
