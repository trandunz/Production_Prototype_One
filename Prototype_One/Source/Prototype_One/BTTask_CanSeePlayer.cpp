#include "BTTask_CanSeePlayer.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/PrototypeEnemy.h"
#include "Characters/Prototype_OneCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_CanSeePlayer::UBTTask_CanSeePlayer()
{
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_CanSeePlayer, BlackboardKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_CanSeePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto* aiController = OwnerComp.GetAIOwner();
	auto* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	aiController->GetBlackboardComponent()->SetValueAsObject(BlackboardKey.SelectedKeyName, player);
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

FString UBTTask_CanSeePlayer::GetStaticDescription() const
{
	return Super::GetStaticDescription();
}
