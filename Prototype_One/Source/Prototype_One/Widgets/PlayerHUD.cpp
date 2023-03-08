#include "PlayerHUD.h"

#include "DebugMenu.h"
#include "GameplayTask.h"
#include "InventoryWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype_One/Characters/Prototype_OneCharacter.h"
#include "Prototype_One/Components/RPGEntityComponent.h"
#include "Prototype_One/Fade.h"

void UPlayerHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	DebugMenu->SetVisibility(ESlateVisibility::Hidden);
	fade = new Fade(BlackFade, 2.0f, 0.0f);
}

void UPlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (fade)
	{
		fade->Tick(InDeltaTime);
	}

	if (auto* player = Cast<APrototype_OneCharacter>(GetOwningPlayer()->GetCharacter()))
	{
		UpdateStamina(player->EntityComponent->Properties.CurrentStamina, player->EntityComponent->Properties.MaxStamina);
		UpdateHealth(player->EntityComponent->Properties.CurrentHealth, player->EntityComponent->Properties.MaxHealth);
	}
}

void UPlayerHUD::UpdateHealth(int _health, int _maxHealth)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(static_cast<float>(_health) / static_cast<float>(_maxHealth));
	}
}

void UPlayerHUD::UpdateMana(int _mana, int _maxMana)
{
	if (ManaBar)
	{
		ManaBar->SetPercent(static_cast<float>(_mana) / static_cast<float>(_maxMana));
	}
}

void UPlayerHUD::UpdateStamina(int _stamina, int _maxStamina)
{
	if (StaminaBar)
	{
		StaminaBar->SetPercent(static_cast<float>(_stamina) / static_cast<float>(_maxStamina));
	}
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

void UPlayerHUD::UpdateSneakStatus(int _seen)
{
	if (_seen == 0 && SeenImage)
	{
		Crosshair_Overlay->SetVisibility(ESlateVisibility::Visible);
		Crosshair_Overlay->SetBrushFromTexture(SeenImage);
	}
	else if (_seen == 1 && HiddenImage)
	{
		Crosshair_Overlay->SetVisibility(ESlateVisibility::Visible);
		Crosshair_Overlay->SetBrushFromTexture(HalfHiddenImage);
	}
	else if (_seen == 2 && HiddenImage)
	{
		Crosshair_Overlay->SetVisibility(ESlateVisibility::Visible);
		Crosshair_Overlay->SetBrushFromTexture(HiddenImage);
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

void UPlayerHUD::FadeIn()
{
	if (fade)
	{
		fade->FadeIn();
	}
}

void UPlayerHUD::FadeOut()
{
	if (fade)
	{
		fade->FadeOut();
	}
}

void UPlayerHUD::ChangeFadeTime(float _fadeTime)
{
	if (fade)
	{
		fade->ChangeFadeTime(_fadeTime);
	}
}
