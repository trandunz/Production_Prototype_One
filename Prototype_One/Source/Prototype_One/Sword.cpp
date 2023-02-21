#include "Sword.h"
#include "Characters/Prototype_OneCharacter.h"
#include "Kismet/GameplayStatics.h"

ASword::ASword()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASword::BeginPlay()
{
	Super::BeginPlay();
	Mesh->SetRenderCustomDepth(true);
}

void ASword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateInteractionOutline();

}

void ASword::UpdateInteractionOutline()
{
	auto distanceToPlayer = (UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation() - GetActorLocation()).Length();
	if (distanceToPlayer <= InteractionRange && !IsEquiped)
	{
		Mesh->SetRenderCustomDepth(true);
		Mesh->CustomDepthStencilValue = 1;
	}
	else
	{
		Mesh->SetRenderCustomDepth(false);
		Mesh->CustomDepthStencilValue = 1;
	}
}

void ASword::Interact()
{
	if (!IsEquiped)
	{
		IsEquiped = true;
		if (auto* charatcer = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
		{
			AttachToComponent(charatcer->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("hand_r"));
		}
	}
	
}