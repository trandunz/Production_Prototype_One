#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_MoveToLocation.generated.h"

UCLASS()
class PROTOTYPE_ONE_API UBTTask_MoveToLocation : public UBTTask_MoveTo
{
	GENERATED_BODY()
public:
	UBTTask_MoveToLocation();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
