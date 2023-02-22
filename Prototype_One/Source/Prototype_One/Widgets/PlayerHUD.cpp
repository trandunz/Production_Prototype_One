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

void UPlayerHUD::UpdateHealthText(int _currentHealth)
{
	if (HealthText)
	{
		HealthText->SetText(FText::FromString(FString::FromInt(_currentHealth)));
	}
}
