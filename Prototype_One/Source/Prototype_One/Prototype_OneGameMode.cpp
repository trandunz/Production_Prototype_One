#include "Prototype_OneGameMode.h"
#include "Characters/Prototype_OneCharacter.h"
#include "UObject/ConstructorHelpers.h"

APrototype_OneGameMode::APrototype_OneGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_PrototypeCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
