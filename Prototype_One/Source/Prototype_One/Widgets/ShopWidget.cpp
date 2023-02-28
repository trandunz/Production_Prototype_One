#include "ShopWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype_One/Characters/Prototype_OneCharacter.h"
#include "Prototype_One/Components/RPGEntityComponent.h"

void UShopWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Sell->OnPressed.AddDynamic(this, &UShopWidget::SellAnyItems);
	GoBack->OnPressed.AddDynamic(this, &UShopWidget::Back);
	UpgradeHealth->OnPressed.AddDynamic(this, &UShopWidget::OnUpgradeHealth);
	UpgradeStamina->OnPressed.AddDynamic(this, &UShopWidget::OnUpgradeStamina);
	UpgradeCarryWeight->OnPressed.AddDynamic(this, &UShopWidget::OnUpgradeCarryWeight);
}

void UShopWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (player->ValuablesCount > 0)
		{
			Sell->SetIsEnabled(true);
		}
		else
		{
			Sell->SetIsEnabled(false);
		}
		
		Money->SetText(FText::FromString(FString::FromInt(player->EntityComponent->CurrentMoney)));

		if (player->EntityComponent->CurrentMoney >= player->EntityComponent->UpgradeCost * player->EntityComponent->HealthCurrentLevel)
		{
			UpgradeHealth->SetIsEnabled(true);
		}
		else
		{
			UpgradeHealth->SetIsEnabled(false);
		}

		if (player->EntityComponent->CurrentMoney >= player->EntityComponent->UpgradeCost * player->EntityComponent->StaminaCurrentLevel)
		{
			UpgradeStamina->SetIsEnabled(true);
		}
		else
		{
			UpgradeStamina->SetIsEnabled(false);
		}

		if (player->EntityComponent->CurrentMoney >= player->EntityComponent->UpgradeCost * player->EntityComponent->CarryWightCurrentLevel)
		{
			UpgradeCarryWeight->SetIsEnabled(true);
		}
		else
		{
			UpgradeCarryWeight->SetIsEnabled(false);
		}
	}
	
}

void UShopWidget::SellAnyItems()
{
	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (player->ValuablesCount > 0)
		{
			for(int i = 0; i < player->ValuablesCount; i++)
			{
				player->ValuablesCount--;
				player->EntityComponent->CurrentMoney += 10;
			}
		}
	}
}

void UShopWidget::Back()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UShopWidget::OnUpgradeHealth()
{
	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		player->EntityComponent->UpgradeHealth();
	}
}

void UShopWidget::OnUpgradeStamina()
{
	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		player->EntityComponent->UpgradeStamina();
	}
}

void UShopWidget::OnUpgradeCarryWeight()
{
	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		player->EntityComponent->UpgradeCarryWeight();
	}
}
