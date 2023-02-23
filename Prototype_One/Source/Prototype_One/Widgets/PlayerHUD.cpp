#include "PlayerHUD.h"

#include "DebugMenu.h"
#include "GameplayTask.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype_One/Characters/Prototype_OneCharacter.h"
#include "Prototype_One/Components/RPGEntityComponent.h"

void UPlayerHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	DebugMenu->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
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


void UPlayerHUD::ToggleDebugMenu()
{
	if (DebugMenu)
	{
		if (DebugMenu->GetVisibility() == ESlateVisibility::Hidden)
			DebugMenu->SetVisibility(ESlateVisibility::Visible);
		else
			DebugMenu->SetVisibility(ESlateVisibility::Hidden);
	}
}
