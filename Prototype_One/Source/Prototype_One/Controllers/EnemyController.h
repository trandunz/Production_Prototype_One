#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"


#include "EnemyController.generated.h"

UCLASS()
class PROTOTYPE_ONE_API AEnemyController : public AAIController
{
	GENERATED_BODY()
public:
	AEnemyController();

protected:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AI", meta = (AllowPrivateAccess = true))
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI", meta = (AllowPrivateAccess = true))
	class UBehaviorTreeComponent* BehaviorTreeComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI", meta = (AllowPrivateAccess = true))
	class UBlackboardComponent* BlackboardComponent;

	class UAISenseConfig_Sight* SightConfig;


	
	float DetectionTimer{};
	UPROPERTY(EditAnywhere)
	float DetectionTime{2.0f};
	float Dt{};
	bool CanSeePlayer{};
	
	UFUNCTION()
	void OnUpdated(AActor* actor, FAIStimulus const stimulus);
	void SetupPerceptionSystem();
};
