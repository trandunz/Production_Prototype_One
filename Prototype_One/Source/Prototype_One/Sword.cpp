#include "Sword.h"

#include "Characters/PrototypeEnemy.h"
#include "Characters/Prototype_OneCharacter.h"
#include "Components/RPGEntityComponent.h"
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

	Mesh->SetCollisionProfileName("Trigger");
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &ASword::OnHit);
}

void ASword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateInteractionOutline();

}

void ASword::UpdateInteractionOutline()
{
	if (auto* character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		auto distanceToPlayer = (character->GetActorLocation() - GetActorLocation()).Length();
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
	
}

void ASword::Interact()
{
	if (!IsEquiped)
	{
		IsEquiped = true;
		if (auto* charatcer = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
		{
			AttachToComponent(charatcer->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Sword_Socket"));
			charatcer->CurrentlyHeldActor = this;
		}
	}
	
}

void ASword::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (auto* enemy = Cast<APrototypeEnemy>(OtherActor))
	{
		enemy->TakeDamage(100);
		UE_LOG(LogTemp, Warning, TEXT("Enemy Hit!"));
	}
	//UE_LOG(LogTemp, Warning, TEXT("Sword Hit!"));
}
