#include "PlayerHUD.h"

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
	
	
}

void UPlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		UpdateHealthText(player->EntityComponent->CurrentHealth);
		UpdateMoneyText(player->EntityComponent->CurrentMoney);
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

void UPlayerHUD::UpdateHealthText(int _currentHealth)
{
	if (HealthText)
	{
		HealthText->SetText(FText::FromString(FString::FromInt(_currentHealth) + "HP"));
	}
}

void UPlayerHUD::UpdateMoneyText(int _currentMoney)
{
	if (MoneyText)
	{
		if (_currentMoney == 69)
			MoneyText->SetText(FText::FromString("$" + FString::FromInt(_currentMoney) + " (Nice)"));
		else
			MoneyText->SetText(FText::FromString("$" + FString::FromInt(_currentMoney)));
	}
}
