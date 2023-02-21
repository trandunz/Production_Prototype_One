#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Prototype_One/Interfaces/InteractInterface.h"
#include "PrototypeEnemy.generated.h"

UCLASS()
class PROTOTYPE_ONE_API APrototypeEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	APrototypeEnemy();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
