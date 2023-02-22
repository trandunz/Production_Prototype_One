#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueWidget.generated.h"

UCLASS()
class PROTOTYPE_ONE_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	
	void ShowDialogueBox(bool);
	UFUNCTION()
	void OnNextDialogueLine();
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* DialogueBackground;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* DialogueText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* Dialogue_NextLine;

	UPROPERTY(EditAnywhere)
	TArray<FString> DialogueLines{};

	int CurrentDialogueIndex{};
};
