#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Prototype_One/Interfaces/InteractInterface.h"
#include "Prototype_One/EnemyDrop.h"
#include "Prototype_One/Widgets/HealthBarWidget.h"
#include "Components/WidgetComponent.h"
#include "PrototypeEnemy.generated.h"

UCLASS()
class PROTOTYPE_ONE_API APrototypeEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	APrototypeEnemy();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void TakeDamage(int _amount);
	
	UFUNCTION()
	void OnSeePawn(class APawn* _pawn);

	void Attack();

	void Ragdoll();
	
	UPROPERTY(EditAnywhere)
	class UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(EditAnywhere)
	bool SeenPlayer{};

	UPROPERTY(EditAnywhere)
	class APawn* LastSeenPawn{};

	class UAIPerceptionStimuliSourceComponent* Stimulus;

	UPROPERTY(EditAnywhere)
	UAnimMontage* AttackMontage;
	
	UPROPERTY(EditAnywhere)
	UWidgetComponent* HealthBarWidget;

	UPROPERTY(EditAnywhere)
	class URPGEntityComponent* EntityComponent;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemyDrop> ItemDropPrefab;
};
