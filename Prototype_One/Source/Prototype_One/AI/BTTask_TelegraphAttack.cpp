#include "BTTask_TelegraphAttack.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Prototype_One/Characters/PrototypeEnemy.h"
#include "Prototype_One/Characters/Prototype_OneCharacter.h"
#include "Prototype_One/Controllers/EnemyController.h"
#include "Prototype_One/Controllers/PrototypePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_TelegraphAttack::UBTTask_TelegraphAttack()
{
	NodeName = TEXT("Telegraph Attack");

	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_TelegraphAttack, BlackboardKey));
}

EBTNodeResult::Type UBTTask_TelegraphAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	
	auto* aiController = OwnerComp.GetAIOwner();
	auto* aiPawn = aiController->GetPawn();
	auto* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (auto* enemy  = Cast<APrototypeEnemy>(aiPawn))
	{
		enemy->FlashRedAndThenWhite();
		enemy->SetActorRotation(FRotator{enemy->GetActorRotation().Pitch, UKismetMathLibrary::FindLookAtRotation(enemy->GetActorLocation(), player->GetActorLocation()).Yaw, enemy->GetActorRotation().Roll});
	}
	
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

FString UBTTask_TelegraphAttack::GetStaticDescription() const
{
	return FString::Printf(TEXT("Play Animation"));
}
