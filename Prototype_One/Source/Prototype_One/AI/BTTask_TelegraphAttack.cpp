#include "BTTask_TelegraphAttack.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Prototype_One/Characters/PrototypeEnemy.h"
#include "Prototype_One/Characters/Prototype_OneCharacter.h"
#include "Prototype_One/Controllers/EnemyController.h"
#include "Prototype_One/Controllers/PrototypePlayerController.h"
#include "Kismet/GameplayStatics.h"

UBTTask_TelegraphAttack::UBTTask_TelegraphAttack()
{
	NodeName = TEXT("Telegraph Attack");

	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_TelegraphAttack, BlackboardKey));
}

EBTNodeResult::Type UBTTask_TelegraphAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	
	auto* aiController = OwnerComp.GetAIOwner();
	auto* aiPawn = aiController->GetPawn();
	if (auto* enemy  = Cast<APrototypeEnemy>(aiPawn))
	{
		enemy->FlashRedAndThenWhite();
	}
	
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

FString UBTTask_TelegraphAttack::GetStaticDescription() const
{
	return FString::Printf(TEXT("Play Animation"));
}
