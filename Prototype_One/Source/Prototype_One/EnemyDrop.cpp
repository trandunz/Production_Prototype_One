// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyDrop.h"

#include "ItemComponent.h"
#include "Characters/Prototype_OneCharacter.h"

// Sets default values
AEnemyDrop::AEnemyDrop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void AEnemyDrop::BeginPlay()
{
	Super::BeginPlay();
	Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &AEnemyDrop::OnHit);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	SetActorEnableCollision(true);
}

// Called every frame
void AEnemyDrop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyDrop::Interact()
{
	
}

void AEnemyDrop::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (auto* player = Cast<APrototype_OneCharacter>(OtherActor))
	{
		Destroy();
	}
}

