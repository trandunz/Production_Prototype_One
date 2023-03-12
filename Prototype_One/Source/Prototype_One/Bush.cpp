#include "Bush.h"

#include "Components/SlateWrapperTypes.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

#include "Prototype_One/Item.h"
#include "Widgets/HealthBarWidget.h"

ABush::ABush()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	Carrot1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Carrot1"));
	Carrot1->SetupAttachment(RootComponent);
	Carrot2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Carrot2"));
	Carrot2->SetupAttachment(RootComponent);
	Carrot3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Carrot3"));
	Carrot3->SetupAttachment(RootComponent);
	Carrot4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Carrot4"));
	Carrot4->SetupAttachment(RootComponent);
	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget Info"));
	HealthBarWidget->SetupAttachment(RootComponent);
}

void ABush::BeginPlay()
{
	Super::BeginPlay();
	if (Explosion) {
		// This spawns the chosen effect on the owning WeaponMuzzle SceneComponent
		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(Explosion, Mesh, FName("Base-HumanTail8"), FVector(0.f), FRotator(0.f), EAttachLocation::SnapToTarget, true);
		NiagaraComp->SetPaused(true);
		
	}
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ABush::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	auto* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	Cast<UHealthBarWidget>(HealthBarWidget->GetWidget())->SetHealthPercent(CurrentHealth, 100);
	if (RespawnTimer > 0 && CurrentHealth <= 0)
	{
		RespawnTimer -= DeltaTime;
	}
	else if (RespawnTimer <= 0 && CurrentHealth <= 0 && (player->GetActorLocation() - GetActorLocation()).Length() >= 2000)
	{
		Mesh->SetVisibility(true);
		HealthBarWidget->SetVisibility(false);
		
		Carrot1->SetVisibility(true);
		Carrot2->SetVisibility(true);
		Carrot3->SetVisibility(true);
		Carrot4->SetVisibility(true);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CurrentHealth = 100.0f;
	}

	if (CurrentHealth < 100.0f && RespawnTimer <= 0 && Mesh->GetCollisionEnabled() == ECollisionEnabled::QueryAndPhysics)
	{
		HealthBarWidget->SetVisibility(true);
	}
	else
	{
		HealthBarWidget->SetVisibility(false);
	}
}

void ABush::TakeDamage(int _amount)
{
	if (CurrentHealth > 0)
	{
		CurrentHealth -= _amount;

		if (CurrentHealth <= 0)
		{
			CurrentHealth = 0;
			RespawnTimer = RespawnTime;
			Mesh->SetVisibility(false);
			Carrot1->SetVisibility(false);
			Carrot2->SetVisibility(false);
			Carrot3->SetVisibility(false);
			HealthBarWidget->SetVisibility(false);
			Carrot4->SetVisibility(false);
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			if (NiagaraComp)
				NiagaraComp->SetPaused(false);
			if (CarrotPrefab && StickPrefab)
			{
				for(int i = 0; i < (rand() % 4) + 4; i++)
				{
					auto* newCarrot = GetWorld()->SpawnActor<AActor>(CarrotPrefab);
					FVector randomDeviance{(rand() % 25) - 12.5f, (rand() % 25) - 12.5f, 100};
					newCarrot->SetActorLocation(GetActorLocation() + randomDeviance);
				}
				for(int i = 0; i < (rand() % 2) + 1; i++)
				{
					auto* newStick = GetWorld()->SpawnActor<AActor>(StickPrefab);
					FVector randomDeviance{(rand() % 25) - 12.5f, (rand() % 25) - 12.5f, 100};
					newStick->SetActorLocation(GetActorLocation() + randomDeviance);
				}
			}

			// Call Audio
			BushDestroyedEvent();
		}
		else
		{
			// Call Audio
			BushHitEvent();
		}
	}
}
