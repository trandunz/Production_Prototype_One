#include "BTTask_FleeActor.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "Characters/PrototypeEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/Prototype_OneCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_FleeActor::UBTTask_FleeActor()
{
	NodeName = TEXT("Find Random Location");

	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FleeActor, BlackboardKey));
}

EBTNodeResult::Type UBTTask_FleeActor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FNavLocation location{};

	auto* aiController = OwnerComp.GetAIOwner();
	auto* aiPawn = aiController->GetPawn();
	
	auto origin = aiPawn->GetActorLocation();
	auto* navSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	auto* character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (character)
	{
		FVector vec = (UKismetMathLibrary::GetForwardVector(UKismetMathLibrary::FindLookAtRotation(aiPawn->GetActorLocation(), character->GetActorLocation())) * -2000.0f) + aiPawn->GetActorLocation();
		vec.Z = aiPawn->GetActorLocation().Z;
	
		location.Location = vec;
		Cast<APrototypeEnemy>(aiPawn)->IsFleeing = true;
		aiController->GetBlackboardComponent()->SetValueAsVector(BlackboardKey.SelectedKeyName, location.Location);
	}
	

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

FString UBTTask_FleeActor::GetStaticDescription() const
{
	return FString::Printf(TEXT("Vector: %s"), *BlackboardKey.SelectedKeyName.ToString());
}