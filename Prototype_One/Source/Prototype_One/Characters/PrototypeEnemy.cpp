#include "PrototypeEnemy.h"

#include "NavigationSystem.h"
#include "Prototype_OneCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/PawnSensingComponent.h"
#include "Prototype_One/Gamemodes/Prototype_OneGameMode.h"
#include "Prototype_One/Controllers/EnemyController.h"
#include "Prototype_One/Components/RPGEntityComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Math/UnitConversion.h"
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

	if (CrownPrefab)
	{
		ItemDrop = GetWorld()->SpawnActor(CrownPrefab);
		
		ItemDrop->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Rabbit_Head"));
		ItemDrop->SetActorRelativeScale3D({0.5f,0.5f,0.5f});
		ItemDrop->SetActorRelativeLocation({-0.213160f,3.987725f,4.336831f});
		ItemDrop->SetActorRelativeRotation({14.652504f,28.750578f,86.797552f});
	}
	else if (MaskPrefab)
	{
		ItemDrop = GetWorld()->SpawnActor(MaskPrefab);
		
		ItemDrop->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Rabbit_Head"));
		ItemDrop->SetActorRelativeScale3D({0.2f,0.2f,0.2f});
		ItemDrop->SetActorRelativeLocation({-4.6, -9.6, 0.0f});
		ItemDrop->SetActorRelativeRotation({90.0f, 26.5f, 106.5f});
	}
	else  if (SmallAntlerPrefab)
	{
		ItemDrop = GetWorld()->SpawnActor(SmallAntlerPrefab);
		
		ItemDrop->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Rabbit_Head"));
		ItemDrop->SetActorRelativeScale3D({0.2f,0.2f,0.2f});
		ItemDrop->SetActorRelativeLocation({-6, -9, 0.0f});
		ItemDrop->SetActorRelativeRotation({90.0f, 145.0f, 234.0f});
	}

	if (auto* item = Cast<AItem>(ItemDrop))
	{
		item->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		item->Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
		item->Mesh->SetSimulatePhysics(false);
		item->Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		item->Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APrototypeEnemy::OnHit);

	if (Trail) {
		// This spawns the chosen effect on the owning WeaponMuzzle SceneComponent
		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(Trail, GetMesh(), FName(), FVector(0.f), FRotator(0.f), EAttachLocation::SnapToTarget, true);
		NiagaraComp->SetPaused(false);
		NiagaraComp->SetVisibility(false);
	}
	if (Blood) {
		// This spawns the chosen effect on the owning WeaponMuzzle SceneComponent
		NiagaraBloodComp = UNiagaraFunctionLibrary::SpawnSystemAttached(Blood, GetMesh(), FName(), FVector(0.f), FRotator(0.f), EAttachLocation::SnapToTarget, true);
		NiagaraBloodComp->SetPaused(true);
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

	if (DamageTimer > 0)
		DamageTimer -= DeltaTime;
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

	if (auto* material = GetMesh()->GetMaterial(0))
	{
		if (UMaterialInstanceDynamic* dynamicMaterial = UMaterialInstanceDynamic::Create(material, GetMesh()))
		{
			if (FlashTimer >= 0.5f)
			{
				FlashTimer -= DeltaTime;
				dynamicMaterial->SetVectorParameterValue(FName("Color"), FLinearColor::Green);
			}
			else if (FlashTimer >= 0.1f)
			{
				FlashTimer -= DeltaTime;
				dynamicMaterial->SetVectorParameterValue(FName("Color"), FLinearColor::Yellow);
			}
			else if (FlashTimer > 0.0f)
			{
				FlashTimer -= DeltaTime;
				dynamicMaterial->SetVectorParameterValue(FName("Color"), FLinearColor::Red);
			}
			else
			{
				dynamicMaterial->SetVectorParameterValue(FName("Color"), FLinearColor::White);
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
	if (DamageTimer <= 0)
	{
		DamageTimer = 0.3f;
		if (EntityComponent)
		{
			if (auto* constoller = Cast<AEnemyController>(Controller))
			{
				constoller->CanSeePlayer = true;
				constoller->BlackboardComponent->SetValueAsBool(FName("CanSeePlayer"), true);
			}

			if (EntityComponent->Properties.CurrentHealth > 0)
			{

				EntityComponent->TakeDamage(_amount);
				UE_LOG(LogTemp, Log, TEXT("Hit Enemy"));
				UE_LOG(LogTemp, Log, TEXT("Enemy Health: %s"), *FString::FromInt(EntityComponent->Properties.CurrentHealth));
				if (EntityComponent->Properties.CurrentHealth <= 0)
				{
			
					UE_LOG(LogTemp, Log, TEXT("Kill Enemy"));
					Ragdoll();
				}
				// Trigger for audio in blueprints
				OnEnemyHitEvent();
			}

		}
	}
	
}

void APrototypeEnemy::OnSeePawn(APawn* _pawn)
{
	
}

void APrototypeEnemy::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AttackTimer > 0)
	{
		
		if (auto* sword = Cast<ASword>(OtherComp))
		{
			UE_LOG(LogTemp, Log, TEXT("Enemy Hit Sword"));
		}
		else if (auto* player = Cast<APrototype_OneCharacter>(OtherActor))
		{
			AttackTimer = 0;
			UE_LOG(LogTemp, Log, TEXT("Enemy Hit player"));
			player->TakeDamage(EntityComponent->Properties.AttackDamage);
			
			UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(player->GetMovementComponent());
			if (CharacterComp)
			{
				CharacterComp->Velocity = (player->GetActorLocation() - GetActorLocation()).GetSafeNormal() * 2000.0f;
			}
		}
	}
	
}

void APrototypeEnemy::Attack()
{
	if (AttackMontage)
	{
		if (GetMesh()->GetAnimInstance())
			GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
	}

	const FVector ForwardVector = GetActorForwardVector();

	UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (CharacterComp)
	{
		CharacterComp->Velocity = ForwardVector * 10000.0f;
		if (NiagaraComp)
			NiagaraComp->SetVisibility(true);
			
	}

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// Trigger for audio in blueprints
	OnEnemyAttackEvent();
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

	if (NiagaraBloodComp)
	{
		NiagaraBloodComp->SetPaused(false);
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

void APrototypeEnemy::FlashRedAndThenWhite()
{
	FlashTimer = 1.0f;
}

