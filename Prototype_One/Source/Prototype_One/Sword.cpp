#include "Sword.h"

#include "Characters/PrototypeEnemy.h"
#include "Characters/Prototype_OneCharacter.h"
#include "Components/RPGEntityComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
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


	if (auto* charatcer = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (charatcer->IsAttacking)
		{
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		else
		{
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
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
			Mesh->SetSimulatePhysics(false);
			Mesh->SetCollisionProfileName("Trigger");
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndProbe);
			AttachToComponent(charatcer->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("SwordSocket"));
			Mesh->SetRelativeLocation({});
			charatcer->CurrentlyHeldActor = this;
		}
	}
	
}

void ASword::Unequip()
{
	Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetSimulatePhysics(true);
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	IsEquiped = false;
}

void ASword::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (auto* enemy = Cast<APrototypeEnemy>(OtherActor))
	{
		if (auto* character = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
		{
			if (character->IsAttacking == true)
			{
				enemy->TakeDamage(character->EntityComponent->Properties.AttackDamage);
				UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(enemy->GetMovementComponent());
				if (CharacterComp)
				{
					CharacterComp->Velocity = (enemy->GetActorLocation() - character->GetActorLocation()).GetSafeNormal() * 3000.0f;
				}
				UE_LOG(LogTemp, Warning, TEXT("Enemy Hit!"));
			}
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("Sword Hit!"));
}
