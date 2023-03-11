#include "BTTask_IsPlayerStillInRange.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Prototype_One/Characters/PrototypeEnemy.h"
#include "Prototype_One/Characters/Prototype_OneCharacter.h"
#include "Prototype_One/Controllers/EnemyController.h"
#include "Prototype_One/Controllers/PrototypePlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_IsPlayerStillInRange::UBTTask_IsPlayerStillInRange()
{
	NodeName = TEXT("StopEnemyAttack");
}

EBTNodeResult::Type UBTTask_IsPlayerStillInRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto* aiController = OwnerComp.GetAIOwner();
	if (auto* enemy = Cast<APrototypeEnemy>(aiController->GetCharacter()))
	{
		enemy->AttackTimer = 0.0f;
	}
	
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

FString UBTTask_IsPlayerStillInRange::GetStaticDescription() const
{
	return FString::Printf(TEXT("Bool"));
}
