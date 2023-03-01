#include "ShopWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype_One/Prototype_OneGameMode.h"
#include "Prototype_One/Characters/Prototype_OneCharacter.h"
#include "Prototype_One/Components/RPGEntityComponent.h"
#include "Prototype_One/Controllers/PrototypePlayerController.h"
#include "Styling/SlateBrush.h"

void UShopWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Sell->OnPressed.AddDynamic(this, &UShopWidget::SellAnyItems);
	GoBack->OnPressed.AddDynamic(this, &UShopWidget::Back);
	UpgradeHealth->OnPressed.AddDynamic(this, &UShopWidget::OnUpgradeHealth);
	UpgradeStamina->OnPressed.AddDynamic(this, &UShopWidget::OnUpgradeStamina);
	UpgradeCarryWeight->OnPressed.AddDynamic(this, &UShopWidget::OnUpgradeCarryWeight);

	BuyHealthPotion->OnPressed.AddDynamic(this, &UShopWidget::OnBuyHealthPotion);
	BuyStaminaPotion->OnPressed.AddDynamic(this, &UShopWidget::OnBuyStaminaPotion);


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
		
		Money->SetText(FText::FromString(FString::FromInt(player->EntityComponent->Properties.CurrentMoney)));

		if (player->EntityComponent->Properties.CurrentMoney >= player->EntityComponent->Properties.UpgradeCost * player->EntityComponent->Properties.HealthCurrentLevel)
		{
			UpgradeHealth->SetIsEnabled(true);
		}
		else
		{
			UpgradeHealth->SetIsEnabled(false);
		}

		if (player->EntityComponent->Properties.CurrentMoney >= player->EntityComponent->Properties.UpgradeCost * player->EntityComponent->Properties.StaminaCurrentLevel)
		{
			UpgradeStamina->SetIsEnabled(true);
		}
		else
		{
			UpgradeStamina->SetIsEnabled(false);
		}

		if (player->EntityComponent->Properties.CurrentMoney >= player->EntityComponent->Properties.UpgradeCost * player->EntityComponent->Properties.CarryWeightCurrentLevel)
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
				player->EntityComponent->Properties.CurrentMoney += 10;
			}
		}
	}
}

void UShopWidget::Back()
{
	SetVisibility(ESlateVisibility::Hidden);

	if (auto* gamemode = Cast<APrototype_OneGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
		{
			if (gamemode->SavedPlayerData)
			{
				gamemode->SavedPlayerData->SavedProperties = player->EntityComponent->Properties;
			}
		}
		gamemode->SaveGame();
	}
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

void UShopWidget::OnBuyStaminaPotion()
{
	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		// buy Stamina potion
	}
}

void UShopWidget::OnBuyHealthPotion()
{
	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		// buy health potion
	}
}
