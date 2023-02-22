#include "PrototypeEnemy.h"

#include "NavigationSystem.h"
#include "Prototype_OneCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/PawnSensingComponent.h"
#include "Prototype_One/Controllers/EnemyController.h"

APrototypeEnemy::APrototypeEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing Component"));
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &APrototypeEnemy::OnSeePawn);
	Stimulus = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	Stimulus->RegisterForSense(TSubclassOf<UAISense_Sight>());
	Stimulus->RegisterWithPerceptionSystem();
}

void APrototypeEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void APrototypeEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APrototypeEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APrototypeEnemy::OnSeePawn(APawn* _pawn)
{
	//LastSeenPawn = _pawn;
	//
	//if (auto* character = Cast<APrototype_OneCharacter>(_pawn))
	//{
	//	if (auto * aiInstance =
	//	Cast<AEnemyController>(Controller))
	//	{
	//		aiInstance->MoveToLocation(character->GetOwner()->GetActorLocation());
	//	}
	//}
}

