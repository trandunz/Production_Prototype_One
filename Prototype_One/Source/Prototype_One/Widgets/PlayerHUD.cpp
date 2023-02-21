#include "PlayerHUD.h"

#include "GameplayTask.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UPlayerHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Dialogue_NextLine->OnPressed.AddDynamic(this, &UPlayerHUD::OnNextDialogueLine);
	ShowDialogueBox(false);
}

void UPlayerHUD::UpdateInteractionText(FString _interactionKey, FString _message)
{
	if (InteractionText)
	{
		float mouseX;
		float mouseY;
		auto* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		controller->GetMousePosition(mouseX, mouseY);
		
		InteractionText->SetText(FText::FromString(_interactionKey == "" ? "" : "Press " + _interactionKey + " " + _message));
	}
}

void UPlayerHUD::ShowDialogueBox(bool _show)
{
	if (_show)
	{
		//DialogueText->SetText(FText{});
		DialogueBackground->SetVisibility(ESlateVisibility::Visible);
		if (auto* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			playerController->SetInputMode(FInputModeUIOnly{});
			playerController->SetIgnoreMoveInput(true);
		}
	}
	else
	{
		DialogueText->SetText(FText{});
		DialogueBackground->SetVisibility(ESlateVisibility::Hidden);
		if (auto* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			playerController->SetInputMode(FInputModeGameAndUI{});
			playerController->SetIgnoreMoveInput(false);
		}
	}
}

void UPlayerHUD::OnNextDialogueLine()
{
	UE_LOG(LogTemp, Warning, TEXT("Next Dialogue Queue Please") );
	
	if (DialogueBackground->GetVisibility() == ESlateVisibility::Visible)
		ShowDialogueBox(false);
}