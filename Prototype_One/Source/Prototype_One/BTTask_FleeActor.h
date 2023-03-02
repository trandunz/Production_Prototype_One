#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FleeActor.generated.h"

UCLASS()
class PROTOTYPE_ONE_API UBTTask_FleeActor : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_FleeActor();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
