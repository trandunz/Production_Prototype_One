#include "Rock.h"

#include "Components/SlateWrapperTypes.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
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
	if (Explosion) {
		// This spawns the chosen effect on the owning WeaponMuzzle SceneComponent
		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(Explosion, Mesh, FName("Base-HumanTail8"), FVector(0.f), FRotator(0.f), EAttachLocation::SnapToTarget, true);
		NiagaraComp->SetPaused(true);
	}
}

void ARock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	auto* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (RespawnTimer > 0 && CurrentHealth <= 0)
	{
		RespawnTimer -= DeltaTime;
	}
	else if (RespawnTimer <= 0 && CurrentHealth <= 0&& (player->GetActorLocation() - GetActorLocation()).Length() >= 2000)
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
			if (NiagaraComp)
				NiagaraComp->SetPaused(false);
			
			if (RockPrefab)
			{
				for(int i = 0; i < (rand() % 4) + 4; i++)
				{
					auto* newCarrot = GetWorld()->SpawnActor<AActor>(RockPrefab);
					FVector randomDeviance{(rand() % 25) - 12.5f, (rand() % 25) - 12.5f, 100};
					newCarrot->SetActorLocation(GetActorLocation() + randomDeviance);
				}
			}

			// Audio event
			RockDestroyedEvent();
		}
		else
		{
			// Audio event
			RockHitEvent();
		}
	}
}

