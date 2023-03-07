#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_Attack.generated.h"

UCLASS()
class PROTOTYPE_ONE_API UBTTask_Attack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTTask_Attack();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
