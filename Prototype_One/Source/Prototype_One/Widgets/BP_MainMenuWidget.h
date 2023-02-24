#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BP_MainMenuWidget.generated.h"

UCLASS()
class PROTOTYPE_ONE_API UBP_MainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
};
