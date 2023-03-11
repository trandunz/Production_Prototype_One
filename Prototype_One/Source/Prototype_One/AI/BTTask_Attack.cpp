#include "BTTask_Attack.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Prototype_One/Characters/PrototypeEnemy.h"
#include "Prototype_One/Characters/Prototype_OneCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto* aiController = OwnerComp.GetAIOwner();
	if (auto* enemy = Cast<APrototypeEnemy>(aiController->GetCharacter()))
	{
		enemy->Attack();
		enemy->AttackTimer = 1.0f;
	}
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

FString UBTTask_Attack::GetStaticDescription() const
{
	return FString::Printf(TEXT("NULL"));
}
