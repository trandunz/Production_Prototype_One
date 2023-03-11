#include "Rock.h"

#include "Components/SlateWrapperTypes.h"
#include "Prototype_One/Item.h"
ARock::ARock()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

void ARock::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (RespawnTimer > 0 && CurrentHealth <= 0)
	{
		RespawnTimer -= DeltaTime;
	}
	else if (RespawnTimer <= 0 && CurrentHealth <= 0)
	{
		Mesh->SetVisibility(true);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CurrentHealth = 200.0f;
	}
}

void ARock::TakeDamage(int _amount)
{
	if (CurrentHealth > 0)
	{
		CurrentHealth -= _amount;

		if (CurrentHealth <= 0)
		{
			CurrentHealth = 0;
			RespawnTimer = RespawnTime;
			Mesh->SetVisibility(false);
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			if (RockPrefab)
			{
				for(int i = 0; i < (rand() % 4) + 4; i++)
				{
					auto* newCarrot = GetWorld()->SpawnActor<AActor>(RockPrefab);
					FVector randomDeviance{(rand() % 25) - 12.5f, (rand() % 25) - 12.5f, 100};
					newCarrot->SetActorLocation(GetActorLocation() + randomDeviance);
				}
			}
		}
	}
}

