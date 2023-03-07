#include "BTTask_GetPlayerActor.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Prototype_One/Characters/PrototypeEnemy.h"
#include "Prototype_One/Characters/Prototype_OneCharacter.h"
#include "Prototype_One/Controllers/EnemyController.h"
#include "Prototype_One/Controllers/PrototypePlayerController.h"
#include "Kismet/GameplayStatics.h"

UBTTask_GetPlayerActor::UBTTask_GetPlayerActor()
{
	NodeName = TEXT("Get Player Actor");

	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GetPlayerActor, BlackboardKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_GetPlayerActor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto* aiController = OwnerComp.GetAIOwner();
	if (auto* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		aiController->GetBlackboardComponent()->SetValueAsObject(BlackboardKey.SelectedKeyName, player);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}
	
	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	return EBTNodeResult::Failed;
}

FString UBTTask_GetPlayerActor::GetStaticDescription() const
{
	return FString::Printf(TEXT("Get Player Actor"));
}
