#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_CanSeePlayer.generated.h"

UCLASS()
class PROTOTYPE_ONE_API UBTTask_CanSeePlayer : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTTask_CanSeePlayer();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
