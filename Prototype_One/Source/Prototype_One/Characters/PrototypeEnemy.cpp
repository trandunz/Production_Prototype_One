#include "PrototypeEnemy.h"

APrototypeEnemy::APrototypeEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

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

