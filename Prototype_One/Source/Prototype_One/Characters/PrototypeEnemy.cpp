#include "PrototypeEnemy.h"

#include "NavigationSystem.h"
#include "Prototype_OneCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/PawnSensingComponent.h"
#include "Prototype_One/Prototype_OneGameMode.h"
#include "Prototype_One/Controllers/EnemyController.h"
#include "Prototype_One/Components/RPGEntityComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


APrototypeEnemy::APrototypeEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing Component"));
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &APrototypeEnemy::OnSeePawn);
	Stimulus = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	Stimulus->RegisterForSense(TSubclassOf<UAISense_Sight>());
	Stimulus->RegisterWithPerceptionSystem();
	EntityComponent = CreateDefaultSubobject<URPGEntityComponent>(TEXT("Entity Component"));
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

void APrototypeEnemy::TakeDamage(int _amount)
{
	if (EntityComponent)
	{
		EntityComponent->TakeDamage(_amount);
		if (EntityComponent->CurrentHealth <= 0)
		{
			Ragdoll();
		}
	}
}

void APrototypeEnemy::OnSeePawn(APawn* _pawn)
{
	//LastSeenPawn = _pawn;
	//
	//if (auto* character = Cast<APrototype_OneCharacter>(_pawn))
	//{
	//	if (auto * aiInstance =
	//	Cast<AEnemyController>(Controller))
	//	{
	//		aiInstance->MoveToLocation(character->GetOwner()->GetActorLocation());
	//	}
	//}
}

void APrototypeEnemy::Attack()
{
	if (AttackMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);

		if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
		{
			player->TakeDamage(34);
		}
	}
}

void APrototypeEnemy::Ragdoll()
{
	if (ItemDropPrefab)
	{
		auto* itemDrop = GetWorld()->SpawnActor(ItemDropPrefab);
		itemDrop->SetActorLocation(GetActorLocation() + FVector{0,0,300});
		itemDrop->SetActorScale3D({0.1f,0.1f,0.1f});
	}
	
	
	SetReplicateMovement(false);

	DetachFromControllerPendingDestroy();
	
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	SetActorEnableCollision(true);
	
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;

	UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (CharacterComp)
	{
		CharacterComp->StopMovementImmediately();
		CharacterComp->DisableMovement();
		CharacterComp->SetComponentTickEnabled(false);
	}

	SetLifeSpan(10.0f);
}

