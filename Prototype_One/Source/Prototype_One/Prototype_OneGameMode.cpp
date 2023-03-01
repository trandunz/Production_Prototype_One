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

void APrototype_OneGameMode::BeginPlay()
{
	Super::BeginPlay();

	FString levelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	if (UGameplayStatics::DoesSaveGameExist(levelName, 0))
	{
		SavedPlayerData = Cast<USavedPlayerData>(UGameplayStatics::LoadGameFromSlot(levelName, 0));
	}
	else if (SavedPlayerDataPrefab)
	{
		SavedPlayerData = Cast<USavedPlayerData>(UGameplayStatics::CreateSaveGameObject(SavedPlayerDataPrefab));
	}

	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (SavedPlayerData)
		{
			if (SavedPlayerData->SavedProperties.CurrentHealth > 0)
			{
				player->EntityComponent->Properties = SavedPlayerData->SavedProperties;
			}
		}
	}
}

void APrototype_OneGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	SaveGame();
}
