#include "Tree.h"

ATree::ATree()
{
	PrimaryActorTick.bCanEverTick = true;

	Stump = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stump"));
	Stump->SetupAttachment(RootComponent);
	Trunk = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Trunk"));
	Trunk->SetupAttachment(Stump);
	Leaves = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Leaves"));
	Leaves->SetupAttachment(Trunk);
}

void ATree::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

