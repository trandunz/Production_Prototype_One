#include "BTTask_CanSeePlayer.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/PrototypeEnemy.h"
#include "Characters/Prototype_OneCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_CanSeePlayer::UBTTask_CanSeePlayer()
{
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_CanSeePlayer, BlackboardKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_CanSeePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto* aiController = OwnerComp.GetAIOwner();
	auto* aiPawn = aiController->GetPawn();

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	auto* enemy = Cast<APrototypeEnemy>(aiPawn);
	if (enemy->SeenPlayer)
	{
		auto* lastSeenPawn = Cast<APrototypeEnemy>(aiPawn)->LastSeenPawn;
		if ((lastSeenPawn->GetActorLocation() - aiPawn->GetActorLocation()).Length() > 1500.0f)
		{
			enemy->GetCharacterMovement()->MaxWalkSpeed = 100.0f;
			return EBTNodeResult::Failed;
		}
		else
		{
			enemy->GetCharacterMovement()->MaxWalkSpeed = 400.0f;
			aiController->GetBlackboardComponent()->SetValueAsObject(BlackboardKey.SelectedKeyName, Cast<APrototypeEnemy>(aiPawn)->LastSeenPawn);
			return EBTNodeResult::Succeeded;
		}
	}
	else
	{
		enemy->GetCharacterMovement()->MaxWalkSpeed = 100.0f;
		return EBTNodeResult::Failed;
	}
}

FString UBTTask_CanSeePlayer::GetStaticDescription() const
{
	return Super::GetStaticDescription();
}
