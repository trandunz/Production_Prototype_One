#include "DialogueWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UDialogueWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	Dialogue_NextLine->OnPressed.AddDynamic(this, &UDialogueWidget::OnNextDialogueLine);
	ShowDialogueBox(false);
}

void UDialogueWidget::ShowDialogueBox(bool _show)
{
	if (_show)
	{
		DialogueText->SetText(FText::FromString(DialogueLines[CurrentDialogueIndex]));
		SetVisibility(ESlateVisibility::Visible);
		CurrentDialogueIndex = 0;
		if (auto* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			playerController->SetInputMode(FInputModeUIOnly{});
			playerController->SetIgnoreMoveInput(true);
		}
	}
	else
	{
		DialogueText->SetText(FText::FromString(DialogueLines[0]));
		SetVisibility(ESlateVisibility::Hidden);
		CurrentDialogueIndex = 0;
		if (auto* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			playerController->SetInputMode(FInputModeGameAndUI{});
			playerController->SetIgnoreMoveInput(false);
		}
	}
}

void UDialogueWidget::OnNextDialogueLine()
{
	UE_LOG(LogTemp, Warning, TEXT("Next Dialogue Queue Please") );
	if (CurrentDialogueIndex >= DialogueLines.Num())
	{
		if (DialogueBackground->GetVisibility() == ESlateVisibility::Visible)
		{
			ShowDialogueBox(false);
		}
			
	}
	else if (CurrentDialogueIndex < DialogueLines.Num())
	{
		DialogueText->SetText(FText::FromString(DialogueLines[CurrentDialogueIndex]));
		CurrentDialogueIndex++;
	}
}
