#include "NPCController.h"

ANPCController::ANPCController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behavior Tree Component"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
}

void ANPCController::BeginPlay()
{
	Super::BeginPlay();

	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
		BehaviorTreeComponent->StartTree(*BehaviorTree);
	}
}

void ANPCController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (Blackboard && BehaviorTree)
	{
		if (BehaviorTree->BlackboardAsset)
			Blackboard->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	}
}
