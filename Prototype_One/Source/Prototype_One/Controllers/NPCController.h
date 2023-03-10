#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NPCController.generated.h"

UCLASS()
class PROTOTYPE_ONE_API ANPCController : public AAIController
{
	GENERATED_BODY()
public:
	ANPCController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AI", meta = (AllowPrivateAccess = true))
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI", meta = (AllowPrivateAccess = true))
	class UBehaviorTreeComponent* BehaviorTreeComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI", meta = (AllowPrivateAccess = true))
	class UBlackboardComponent* BlackboardComponent;
};
