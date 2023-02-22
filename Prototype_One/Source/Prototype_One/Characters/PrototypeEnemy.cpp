#include "PrototypeEnemy.h"

#include "Prototype_OneCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/PawnSensingComponent.h"

APrototypeEnemy::APrototypeEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing Component"));
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &APrototypeEnemy::OnSeePawn);
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
	LastSeenPawn = _pawn;
	if (auto* character = Cast<APrototype_OneCharacter>(_pawn))
	{
		SeenPlayer = true;
	}
}

