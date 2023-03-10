#include "BTTask_FindRandomLocation.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Prototype_One/Characters/PrototypeEnemy.h"
#include "Prototype_One/Characters/Prototype_OneCharacter.h"
#include "Prototype_One/Controllers/EnemyController.h"
#include "Prototype_One/Controllers/PrototypePlayerController.h"
#include "Kismet/GameplayStatics.h"

UBTTask_FindRandomLocation::UBTTask_FindRandomLocation()
{
	NodeName = TEXT("Find Random Location");

	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindRandomLocation, BlackboardKey));
}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FNavLocation location{};

	auto* aiController = OwnerComp.GetAIOwner();
	auto* aiPawn = aiController->GetPawn();
	
	auto origin = aiPawn->GetActorLocation();
	auto* navSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (navSystem && navSystem->GetRandomPointInNavigableRadius(origin, SearchRadius, location))
	{
		Cast<APrototypeEnemy>(aiPawn)->IsFleeing = false;
		if (!Cast<APrototypeEnemy>(aiPawn)->SeenPlayer)
		{
			aiController->GetBlackboardComponent()->SetValueAsVector(BlackboardKey.SelectedKeyName, location.Location);
		}
		
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

FString UBTTask_FindRandomLocation::GetStaticDescription() const
{
	return FString::Printf(TEXT("Vector: %s"), *BlackboardKey.SelectedKeyName.ToString());
}
