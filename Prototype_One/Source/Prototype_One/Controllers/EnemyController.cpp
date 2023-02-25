#include "EnemyController.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Prototype_One/Characters/PrototypeEnemy.h"
#include "Prototype_One/Characters/Prototype_OneCharacter.h"
#include "Prototype_One/Widgets/PlayerHUD.h"
#include "Components/WidgetComponent.h"
#include "Prototype_One/Components/RPGEntityComponent.h"

AEnemyController::AEnemyController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behavior Tree Component"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
	SetupPerceptionSystem();

}

void AEnemyController::BeginPlay()
{
	Super::BeginPlay();

	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
		BehaviorTreeComponent->StartTree(*BehaviorTree);
	}


}



void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (Blackboard && BehaviorTree)
	{
		if (BehaviorTree->BlackboardAsset)
			Blackboard->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	}
}

void AEnemyController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Dt = DeltaSeconds;
	if (auto* character = Cast<APrototypeEnemy>(GetCharacter()))
	{
		if (CanSeePlayer && DetectionTimer > 0 && !BlackboardComponent->GetValueAsBool(FName("CanSeePlayer")))
		{
			DetectionTimer -= DeltaSeconds;
		}
		else if (CanSeePlayer && DetectionTimer <= 0 && !BlackboardComponent->GetValueAsBool(FName("CanSeePlayer")))
		{
			BlackboardComponent->SetValueAsBool(FName("CanSeePlayer"), true);
		}
		else if (!CanSeePlayer && DetectionTimer < DetectionTime)
		{
			BlackboardComponent->SetValueAsBool(FName("CanSeePlayer"), false);
			DetectionTimer += DeltaSeconds;
		}
		
		if (BlackboardComponent->GetValueAsBool(FName("CanSeePlayer")))
		{
			character->GetCharacterMovement()->MaxWalkSpeed = 400.0f;
		}
		else
		{
			character->GetCharacterMovement()->MaxWalkSpeed = 100.0f;
		}



	}

	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (player->PlayerHud)
		{
			player->PlayerHud->UpdateSneakStatus(BlackboardComponent->GetValueAsBool(FName("CanSeePlayer")));
		}
	}
}

void AEnemyController::OnUpdated(AActor* actor, FAIStimulus const stimulus)
{
	if (auto* player = Cast<APrototype_OneCharacter>(actor))
	{
		CanSeePlayer = stimulus.WasSuccessfullySensed();
		UE_LOG(LogTemp, Warning, TEXT("Player Seen!") );
		
	}
}

void AEnemyController::SetupPerceptionSystem()
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
	SightConfig->SightRadius = 1500.0f;
	SightConfig->LoseSightRadius = SightConfig->SightRadius + 500.0f;
	SightConfig->PeripheralVisionAngleDegrees = 65.0f;
	SightConfig->SetMaxAge(5.0f);
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 500.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyController::OnUpdated);
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
}
