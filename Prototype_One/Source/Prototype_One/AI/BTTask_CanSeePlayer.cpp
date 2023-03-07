#include "BTTask_CanSeePlayer.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Prototype_One/Characters/PrototypeEnemy.h"
#include "Prototype_One/Characters/Prototype_OneCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include <Prototype_One/Bag.h>

#include "Prototype_One/Controllers/EnemyController.h"

UBTTask_CanSeePlayer::UBTTask_CanSeePlayer()
{
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_CanSeePlayer, BlackboardKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_CanSeePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto* aiController = OwnerComp.GetAIOwner();
	auto* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	aiController->GetBlackboardComponent()->SetValueAsObject(BlackboardKey.SelectedKeyName, player);

	
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABag::StaticClass(), actors);
	for(auto bagActor : actors)
	{
		if (auto* bag = Cast<ABag>(bagActor))
		{
			if (bag->IsOpen)
			{
				Cast<AEnemyController>(aiController)->CanSeePlayer = true;
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	Cast<AEnemyController>(aiController)->CanSeePlayer = true;
	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	return EBTNodeResult::Failed;
}

FString UBTTask_CanSeePlayer::GetStaticDescription() const
{
	return Super::GetStaticDescription();
}
