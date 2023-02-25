#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"


UCLASS()
class PROTOTYPE_ONE_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealthPercent(int _health, int _maxHealth);
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UProgressBar* HealthBar;
};
