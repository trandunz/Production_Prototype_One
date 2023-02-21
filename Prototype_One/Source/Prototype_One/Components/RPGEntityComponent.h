#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPGEntityComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOTYPE_ONE_API URPGEntityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URPGEntityComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
	int CurrentHealth{};
	UPROPERTY(EditAnywhere)
	int MaxHealth{100};
	UPROPERTY(EditAnywhere)
	int CurrentMana{};
	UPROPERTY(EditAnywhere)
	int MaxMana{100};
	UPROPERTY(EditAnywhere)
	int CurrentStamina{};
	UPROPERTY(EditAnywhere)
	int MaxStamina{100};
};
