#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Prototype_One/SavedPlayerData.h"
#include "Prototype_OneGameMode.generated.h"

UCLASS(minimalapi)
class APrototype_OneGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APrototype_OneGameMode();

	void SaveGame();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<USavedPlayerData> SavedPlayerDataPrefab;

	UPROPERTY(VisibleAnywhere)
	USavedPlayerData* SavedPlayerData;
};



