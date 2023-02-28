#include "DebugMenu.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype_One/Characters/PrototypeEnemy.h"
#include "Prototype_One/Components/RPGEntityComponent.h"
#include "Prototype_One/Characters/Prototype_OneCharacter.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/PawnSensingComponent.h"

void UDebugMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	bHasScriptImplementedTick = true;
	CloseButton->OnPressed.AddDynamic(this, &UDebugMenu::OnCloseMenu);
}

void UDebugMenu::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		PlayerHealthText->SetText(FText::FromString("Health: " + FString::FromInt(player->EntityComponent->CurrentHealth)));
		PlayerMoneyText->SetText(FText::FromString("Money: " + FString::FromInt(player->EntityComponent->CurrentMoney)));
		PlayerStaminaText->SetText(FText::FromString("Stamina: " + FString::FromInt(player->EntityComponent->CurrentStamina)));
	}
}

void UDebugMenu::OnCloseMenu()
{
	SetVisibility(ESlateVisibility::Hidden);
}
