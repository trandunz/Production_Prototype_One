#include "ShopWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype_One/Gamemodes/Prototype_OneGameMode.h"
#include "Prototype_One/Characters/Prototype_OneCharacter.h"
#include "Prototype_One/Components/RPGEntityComponent.h"
#include "Prototype_One/Controllers/PrototypePlayerController.h"
#include "Prototype_One/Bag.h"
#include "Prototype_One/Components/PlayerInventory.h"
#include "Styling/SlateBrush.h"

void UShopWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Sell->OnPressed.AddDynamic(this, &UShopWidget::SellAnyItems);
	
	GoBack->OnPressed.AddDynamic(this, &UShopWidget::Back);
	UpgradeHealth->OnPressed.AddDynamic(this, &UShopWidget::OnUpgradeHealth);
	UpgradeStamina->OnPressed.AddDynamic(this, &UShopWidget::OnUpgradeStamina);
	UpgradeCarryWeight->OnPressed.AddDynamic(this, &UShopWidget::OnUpgradeCarryWeight);
	UpgradeAttackDamage->OnPressed.AddDynamic(this, &UShopWidget::OnUpgradeAttackDamage);

	BuyHealthPotion->OnPressed.AddDynamic(this, &UShopWidget::OnBuyHealthPotion);
	//BuyStaminaPotion->OnPressed.AddDynamic(this, &UShopWidget::OnBuyStaminaPotion);
	BuyGateTicket->OnPressed.AddDynamic(this, &UShopWidget::OnBuyGateTicket);
	

	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABag::StaticClass(), actors);
	if (actors.Num() > 0)
	{
		Bag = Cast<ABag>(actors[0]);
	}	
}

void UShopWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (player->PlayerInventory->GetIsShopping())
		{
			int32 UpgradeCost = player->EntityComponent->Properties.UpgradeCost;
			int32 _HealthLevel = player->EntityComponent->Properties.HealthCurrentLevel;
			int32 _StaminaLevel = player->EntityComponent->Properties.StaminaCurrentLevel;
			int32 _CarryWeightLevel = player->EntityComponent->Properties.CarryWeightCurrentLevel;
			int32 _AttackLevel = player->EntityComponent->Properties.AttackDamageLevel;
			
			FText HealthCostText = FText::AsCultureInvariant(FString::FromInt(_HealthLevel * UpgradeCost));
			FText StaminaCostText = FText::AsCultureInvariant(FString::FromInt( _StaminaLevel * UpgradeCost));
			FText CarryCostText = FText::AsCultureInvariant(FString::FromInt( _CarryWeightLevel * UpgradeCost));
			FText AttackCostText = FText::AsCultureInvariant(FString::FromInt( _AttackLevel * UpgradeCost));

			FText HealthLevelText = FText::AsCultureInvariant(FString::FromInt(_HealthLevel));
			FText StaminaLevelText = FText::AsCultureInvariant(FString::FromInt( _StaminaLevel));
			FText CarryLevelText = FText::AsCultureInvariant(FString::FromInt( _CarryWeightLevel));
			FText AttackLevelText = FText::AsCultureInvariant(FString::FromInt( _AttackLevel));
			
			// Upgrade the text showing the cost of upgrades
			HealthCost->SetText(HealthCostText);
			StaminaCost->SetText(StaminaCostText);			
			CarryWeightCost->SetText(CarryCostText);
			AttackCost->SetText(AttackCostText);

			// Upgrade the text showing the stat levels
			HealthLevel->SetText(HealthLevelText);
			StaminaLevel->SetText(StaminaLevelText);
			CarryWeightLevel->SetText(CarryLevelText);
			AttackLevel->SetText(AttackLevelText);
			
		}
		if (player->ValuablesCount > 0)
		{
			Sell->SetIsEnabled(true);
		}
		else
		{
			Sell->SetIsEnabled(false);
		}

		if (player->PlayerInventory->GetCoins() >= player->EntityComponent->Properties.UpgradeCost * player->EntityComponent->Properties.HealthCurrentLevel)
		{
			UpgradeHealth->SetIsEnabled(true);
		}
		else
		{
			UpgradeHealth->SetIsEnabled(false);
		}

		if (player->PlayerInventory->GetCoins() >= player->EntityComponent->Properties.UpgradeCost * player->EntityComponent->Properties.StaminaCurrentLevel)
		{
			UpgradeStamina->SetIsEnabled(true);
		}
		else
		{
			UpgradeStamina->SetIsEnabled(false);
		}

		if (player->PlayerInventory->GetCoins() >= player->EntityComponent->Properties.UpgradeCost * player->EntityComponent->Properties.CarryWeightCurrentLevel)
		{
			UpgradeCarryWeight->SetIsEnabled(true);
		}
		else
		{
			UpgradeCarryWeight->SetIsEnabled(false);
		}

		if (player->PlayerInventory->GetCoins() >= player->EntityComponent->Properties.UpgradeCost * player->EntityComponent->Properties.AttackDamageLevel)
		{
			UpgradeAttackDamage->SetIsEnabled(true);
		}
		else
		{
			UpgradeAttackDamage->SetIsEnabled(false);
		}
	}
	
}

void UShopWidget::SellAnyItems()
{
	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		player->PlayerInventory->SellAll();
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
		int32 CostOfUpgrade = player->EntityComponent->Properties.UpgradeCost * player->EntityComponent->Properties.HealthCurrentLevel;
		if (player->PlayerInventory->GetCoins() >= CostOfUpgrade)
		{
			player->PlayerInventory->SubtractCoins(CostOfUpgrade);
			player->EntityComponent->UpgradeHealth();			
		}
	}
}

void UShopWidget::OnUpgradeStamina()
{
	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		int32 CostOfUpgrade = player->EntityComponent->Properties.UpgradeCost * player->EntityComponent->Properties.StaminaCurrentLevel;
		if (player->PlayerInventory->GetCoins() >= CostOfUpgrade)
		{
			player->PlayerInventory->SubtractCoins(CostOfUpgrade);
			player->EntityComponent->UpgradeStamina();
		}
	}
}

void UShopWidget::OnUpgradeCarryWeight()
{
	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		int32 CostOfUpgrade = player->EntityComponent->Properties.UpgradeCost * player->EntityComponent->Properties.CarryWeightCurrentLevel;
		if (player->PlayerInventory->GetCoins() >= CostOfUpgrade)
		{
			player->PlayerInventory->SubtractCoins(CostOfUpgrade);
			player->EntityComponent->UpgradeCarryWeight();
		}
	}
}

void UShopWidget::OnUpgradeAttackDamage()
{
	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		int32 CostOfUpgrade = player->EntityComponent->Properties.UpgradeCost * player->EntityComponent->Properties.AttackDamageLevel;
		if (player->PlayerInventory->GetCoins() >= CostOfUpgrade)
		{
			player->PlayerInventory->SubtractCoins(CostOfUpgrade);
			player->EntityComponent->UpgradeAttackDamage();
		}
	}
}


void UShopWidget::OnBuyHealthPotion()
{
 //in blueprints
}

void UShopWidget::OnBuyGateTicket()
{
	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (player->PlayerInventory->GetCoins() >= GateTicketPrice)
		{
			player->PlayerInventory->SubtractCoins(GateTicketPrice);
			player->CanWinGame = true;

			TArray<AActor*> actors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABag::StaticClass(), actors);
			for(auto bagActor : actors)
			{
				if (auto* bag = Cast<ABag>(bagActor))
				{
					//bag->IsBiengPulled = false;
					bag->Interact();
				}
			}
			
		}
	}
}

