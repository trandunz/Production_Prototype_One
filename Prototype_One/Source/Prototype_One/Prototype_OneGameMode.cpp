#include "Prototype_OneGameMode.h"
#include "Characters/Prototype_OneCharacter.h"
#include "UObject/ConstructorHelpers.h"

#include "Kismet/GameplayStatics.h"
#include "Prototype_One/SavedPlayerData.h"

APrototype_OneGameMode::APrototype_OneGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_PrototypeCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;

	}
}

void APrototype_OneGameMode::SaveGame()
{
	if (SavedPlayerData)
	{
		FString levelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
		UGameplayStatics::SaveGameToSlot(SavedPlayerData,levelName,  0);
	}
	
}

void APrototype_OneGameMode::ResetSavedData()
{
}

void APrototype_OneGameMode::BeginPlay()
{
	Super::BeginPlay();

	//FString levelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	//if (UGameplayStatics::DoesSaveGameExist(levelName, 0))
	//{
	//	SavedPlayerData = Cast<USavedPlayerData>(UGameplayStatics::LoadGameFromSlot(levelName, 0));
	//}
	//else if (SavedPlayerDataPrefab)
	//{
	//	SavedPlayerData = Cast<USavedPlayerData>(UGameplayStatics::CreateSaveGameObject(SavedPlayerDataPrefab));
	//}
//
	//if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	//{
	//	if (SavedPlayerData)
	//	{
	//		if (SavedPlayerData->SavedProperties.CurrentMoney > 0)
	//		{
	//			auto props = player->EntityComponent->Properties;
	//			props.HealthCurrentLevel = SavedPlayerData->SavedProperties.HealthCurrentLevel;
	//			props.MaxHealth = SavedPlayerData->SavedProperties.MaxHealth;
	//			props.MaxCarryWeight = SavedPlayerData->SavedProperties.MaxCarryWeight;
	//			props.CurrentCarryWeight = SavedPlayerData->SavedProperties.CurrentCarryWeight;
	//			props.CarryWeightCurrentLevel = SavedPlayerData->SavedProperties.CarryWeightCurrentLevel;
	//			props.AttackDamageLevel = SavedPlayerData->SavedProperties.AttackDamageLevel;
	//			props.MaxStamina = SavedPlayerData->SavedProperties.MaxStamina;
	//			props.StaminaCurrentLevel = SavedPlayerData->SavedProperties.StaminaCurrentLevel;
	//			props.CurrentMoney = SavedPlayerData->SavedProperties.CurrentMoney;
	//			props.AttackDamage = SavedPlayerData->SavedProperties.AttackDamage;
	//			player->EntityComponent->Properties = props;
	//		}
	//	}
	//}
}

void APrototype_OneGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	//SaveGame();
}
