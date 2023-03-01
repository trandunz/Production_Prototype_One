#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Prototype_One/Components/RPGEntityComponent.h"
#include "SavedPlayerData.generated.h"


UCLASS()
class PROTOTYPE_ONE_API USavedPlayerData : public USaveGame
{
	GENERATED_BODY()

public:
	FEntityProperties SavedProperties;
};
