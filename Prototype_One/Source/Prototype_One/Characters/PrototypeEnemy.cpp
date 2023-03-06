#include "PrototypeEnemy.h"

#include "NavigationSystem.h"
#include "Prototype_OneCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/PawnSensingComponent.h"
#include "Prototype_One/Prototype_OneGameMode.h"
#include "Prototype_One/Controllers/EnemyController.h"
#include "Prototype_One/Components/RPGEntityComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Prototype_One/Item.h"


APrototypeEnemy::APrototypeEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing Component"));
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &APrototypeEnemy::OnSeePawn);
	Stimulus = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	Stimulus->RegisterForSense(TSubclassOf<UAISense_Sight>());
	Stimulus->RegisterWithPerceptionSystem();
	EntityComponent = CreateDefaultSubobject<URPGEntityComponent>(TEXT("Entity Component"));
	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget Info"));
	HealthBarWidget->SetupAttachment(RootComponent);
}

void APrototypeEnemy::UpdateAttackOutline(bool _showOutline)
{
	if (auto* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		if (_showOutline)
		{
			GetMesh()->SetRenderCustomDepth(true);
			GetMesh()->CustomDepthStencilValue = 1;
		}
		else
		{
			GetMesh()->SetRenderCustomDepth(false);
			GetMesh()->CustomDepthStencilValue = 1;
		}
	}
}

void APrototypeEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (ItemDropPrefab)
	{
		ItemDrop = GetWorld()->SpawnActor(ItemDropPrefab);
		ItemDrop->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		if (auto* item = Cast<AItem>(ItemDrop))
		{
			item->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			item->Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
			item->Mesh->SetSimulatePhysics(false);
			item->Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
			item->Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		}
	}
	
}

void APrototypeEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (HealthBarWidget)
	{
		HealthBarWidget->UnregisterComponent();
		HealthBarWidget->DestroyComponent();
		HealthBarWidget->RemoveFromRoot();
		HealthBarWidget = nullptr;
	}
}

void APrototypeEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HealthBarWidget)
	{
		if (auto* widget = Cast<UHealthBarWidget>(HealthBarWidget->GetWidget()))
		{
			widget->SetHealthPercent(EntityComponent->Properties.CurrentHealth, EntityComponent->Properties.MaxHealth);

			if (auto* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
			{
				if ((player->GetActorLocation() - GetActorLocation()).Length() < 1000.0f)
				{
						widget->SetVisibility(ESlateVisibility::Visible);
					}
				else
				{
					widget->SetVisibility(ESlateVisibility::Hidden);
				}
			}
		}
	}
}

void APrototypeEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APrototypeEnemy::TakeDamage(int _amount)
{
	if (EntityComponent)
	{
		if (auto* constoller = Cast<AEnemyController>(Controller))
		{
			constoller->CanSeePlayer = true;
			constoller->BlackboardComponent->SetValueAsBool(FName("CanSeePlayer"), true);
		}
		EntityComponent->TakeDamage(_amount);
		UE_LOG(LogTemp, Log, TEXT("Hit Enemy"));
		UE_LOG(LogTemp, Log, TEXT("Enemy Health: %s"), *FString::FromInt(EntityComponent->Properties.CurrentHealth));
		if (EntityComponent->Properties.CurrentHealth <= 0)
		{
			
			UE_LOG(LogTemp, Log, TEXT("Kill Enemy"));
			Ragdoll();
		}
	}
}

void APrototypeEnemy::OnSeePawn(APawn* _pawn)
{
	
}

void APrototypeEnemy::Attack()
{
	if (AttackMontage)
	{
		if (GetMesh()->GetAnimInstance())
			GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);

		if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
		{
			player->TakeDamage(34);
		}
	}
}

void APrototypeEnemy::Ragdoll()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->UnregisterComponent();
		HealthBarWidget->DestroyComponent();
		HealthBarWidget->RemoveFromRoot();
		HealthBarWidget = nullptr;
	}
	if (MonsterMeatPrefab)
	{
		auto* meatDrop = GetWorld()->SpawnActor(MonsterMeatPrefab);
		meatDrop->SetActorLocation(GetActorLocation());
		if (auto* meat = Cast<AItem>(meatDrop))
		{
			meat->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			meat->Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
			meat->Mesh->SetSimulatePhysics(true);
			meat->Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
			meat->Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
			meat->IsPickupable = true;
		}
	}

	if (auto* item = Cast<AItem>(ItemDrop))
	{
		item->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		item->Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
		item->Mesh->SetSimulatePhysics(true);
		item->Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		item->Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		item->Mesh->AddImpulse({0,0,100});
		item->IsPickupable = true;
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

