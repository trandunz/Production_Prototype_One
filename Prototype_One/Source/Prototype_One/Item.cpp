#include "Item.h"

#include "NiagaraFunctionLibrary.h"

AItem::AItem()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	if (Trail) {
		// This spawns the chosen effect on the owning WeaponMuzzle SceneComponent
		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(Trail, Mesh, FName(), FVector(0.f), FRotator(0.f), EAttachLocation::SnapToTarget, true);
		NiagaraComp->SetPaused(false);
		NiagaraComp->SetVisibility(true);
	}
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (NiagaraComp)
	{
		if (IsPickupable)
			NiagaraComp->SetVisibility(true);
		else
		{
			NiagaraComp->SetVisibility(false);
		}
	}
}
