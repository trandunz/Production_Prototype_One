#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Prototype_One/Interfaces/InteractInterface.h"
#include "Prototype_One/EnemyDrop.h"
#include "Prototype_One/Widgets/HealthBarWidget.h"
#include "Components/WidgetComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "PrototypeEnemy.generated.h"

UCLASS()
class PROTOTYPE_ONE_API APrototypeEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	APrototypeEnemy();
	void UpdateAttackOutline(bool);
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void TakeDamage(int _amount);
	
	UFUNCTION()
	void OnSeePawn(class APawn* _pawn);

	UFUNCTION()
	void OnHit(	UPrimitiveComponent* OverlappedComponent, 
			AActor* OtherActor, 
			UPrimitiveComponent* OtherComp, 
			int32 OtherBodyIndex, 
			bool bFromSweep, 
			const FHitResult &SweepResult);

	void Attack();

	void Ragdoll();

	void FlashRedAndThenWhite();

	

	UPROPERTY(VisibleAnywhere)
	float FlashTimer{};
	
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
    UAnimMontage* RoarMontage;
	
	UPROPERTY(EditAnywhere)
	UWidgetComponent* HealthBarWidget;

	UPROPERTY(EditAnywhere)
	class URPGEntityComponent* EntityComponent;

	UPROPERTY(EditAnywhere)
	bool IsFleeing{};

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> MonsterMeatPrefab;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> SmallAntlerPrefab;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> CrownPrefab;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> MaskPrefab;
	
	AActor* ItemDrop;

	UPROPERTY(EditAnywhere, Category = "Trail")
	UNiagaraSystem* Trail;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* NiagaraComp;

	UPROPERTY(EditAnywhere, Category = "Trail")
	UNiagaraSystem* Blood;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* NiagaraBloodComp;

	float AttackTimer{};
	
	// Events for audio trigger in blueprints
	UFUNCTION(BlueprintImplementableEvent)
	void OnEnemyHitEvent();
	UFUNCTION(BlueprintImplementableEvent)
	void OnEnemyAttackEvent();

	float DamageTimer{};
};
