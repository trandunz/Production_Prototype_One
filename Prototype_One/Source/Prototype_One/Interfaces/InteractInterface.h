#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

UINTERFACE(MinimalAPI)
class UInteractInterface : public UInterface
{
	GENERATED_BODY()
};

class PROTOTYPE_ONE_API IInteractInterface
{
	GENERATED_BODY()

public:
	virtual void Interact() = 0;
};
