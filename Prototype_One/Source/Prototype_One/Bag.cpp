#include "Bag.h"

#include "Item.h"
#include "NavigationSystem.h"
#include "Characters/Prototype_OneCharacter.h"
#include "Components/RPGEntityComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Rope.h"
#include "CableComponent/Classes/CableComponent.h"
#include "Components/PlayerInventory.h"
#include "Prototype_One/Item.h"
#include "Kismet/KismetMathLibrary.h"

ABag::ABag()
{
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = Mesh;

	//CableComponent->SetAttachEndTo(this, FName(Mesh->GetName()));
}

void ABag::BeginPlay()
{
	Super::BeginPlay();

	if (RopePrefab)
	{
		Rope = Cast<ARope>(GetWorld()->SpawnActor(RopePrefab));
		Rope->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		Rope->SetActorScale3D({0.1f,0.1f,0.1f});
		TArray<UCableComponent*> cableComponents{};
		Rope->GetComponents<UCableComponent>(cableComponents);
		
		if (cableComponents.Num() > 0)
		{
			CableComponent = cableComponents[0];
		}

		if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
		{
			Player = player;
			CableComponent->SetAttachEndToComponent(Player->GetMesh(), FName("hand_l"));
			CableComponent->EndLocation = {};
		}
		
	}
	
}

void ABag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateInteractionOutline();
	AttractItems(DeltaTime);
	SpawnEnemies(DeltaTime);
	SpawnSmallItems(DeltaTime);
	HandleBehaviorBasedOnWeight(DeltaTime);

	if (Rope)
		Rope->SetActorScale3D({1.0f,1.0f,1.0f});
}

void ABag::Interact()
{
	IsBiengPulled = !IsBiengPulled;
}

void ABag::UpdateInteractionOutline()
{
	if (auto* character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		auto distanceToPlayer = (character->GetActorLocation() - GetActorLocation()).Length();
		if (distanceToPlayer <= InteractionRange)
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

void ABag::AttractItems(float DeltaTime)
{
	if (IsOpen && Player)
	{
		int weight = GetWeight();
	
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItem::StaticClass(), actors);
		TArray<AActor*> ItemsInRange;
		for(auto* actor : actors)
		{
			if ((actor->GetActorLocation() - GetActorLocation()).Length() <= SuctionRadius * FMath::Clamp((weight), 1, 99999))
			{
				ItemsInRange.Add(actor);
			}
		}
		
		// For all items in range, check if pickup able then pickup
		for (int Item = 0; Item < ItemsInRange.Num(); ++Item)
		{
			if (auto* ItemCast = Cast<AItem>(ItemsInRange[Item]))
			{
				if (ItemCast->IsPickupable)
				{
					if ((ItemCast->GetActorLocation() - GetActorLocation()).Length() <= PickupRange)
					{
						// Pickup
						Player->PlayerInventory->Pickup(ItemCast->ItemDetails);

						// Destroy
						ItemCast->Destroy();
					}
					else
					{
						ItemCast->Mesh->SetSimulatePhysics(false);
						ItemCast->Mesh->SetCollisionProfileName("Trigger");
						ItemCast->SetActorLocation(FMath::Lerp<FVector>(ItemCast->GetActorLocation(), GetActorLocation(), DeltaTime * 10.0f));
					}
				}
			}
		}
	}
	
}

void ABag::SpawnEnemies(float DeltaTime)
{
	if (Player)
	{
		int weight = GetWeight();
		if (IsOpen && OpenMesh && RabbitPrefab && MaskedPrefab && KingPrefab)
		{
			Mesh->SetStaticMesh(OpenMesh);
			SetActorScale3D(FVector{1,1,1} * FMath::Clamp(weight * 1.0f, 1.0f, 99999));
			
			if (SpawnTimer > 0)
			{
				SpawnTimer -= DeltaTime;
			}
			else if (SpawnTimer <= 0 && weight > 0)
			{
				UE_LOG(LogTemp, Log, TEXT("Enemy Spawned"));
				SpawnTimer = rand() % 3 + 1;
				FNavLocation location{};
				auto origin = GetActorLocation();
				auto* navSystem = UNavigationSystemV1::GetCurrent(GetWorld());
				
				if (Player->PlayerInventory->GetRabbitSpawnCount() > 0)
				{
					for (int i = 0; i < Player->PlayerInventory->GetRabbitSpawnCount(); i++)
					{
						if (navSystem && navSystem->GetRandomPointInNavigableRadius(origin, 2000.0f, location))
						{
							auto* rabbit = GetWorld()->SpawnActor(RabbitPrefab);
							rabbit->SetActorLocation({location.Location.X, location.Location.Y, location.Location.Z + 100});
						}
					}
				}
				if (Player->PlayerInventory->GetMaskedSpawnCount() > 0)
				{
					for (int i = 0; i < Player->PlayerInventory->GetMaskedSpawnCount(); i++)
					{
						if (navSystem && navSystem->GetRandomPointInNavigableRadius(origin, 2000.0f, location))
						{
							auto* masked = GetWorld()->SpawnActor(MaskedPrefab);
							masked->SetActorLocation({location.Location.X, location.Location.Y, location.Location.Z + 100});
						}
					}
				}
				if (Player->PlayerInventory->GetKingSpawnCount() > 0)
				{
					for (int i = 0; i < Player->PlayerInventory->GetKingSpawnCount(); i++)
					{
						if (navSystem && navSystem->GetRandomPointInNavigableRadius(origin, 2000.0f, location))
						{
							auto* king = GetWorld()->SpawnActor(KingPrefab);
							king->SetActorLocation({location.Location.X, location.Location.Y, location.Location.Z + 100});
						}
					}
				}
			}
		}
		else if (ClosedMesh)
		{
			SetActorScale3D(FVector{1,1,1} * FMath::Clamp(weight * 1.0f, 1.0f, 99999));
			Mesh->SetStaticMesh(ClosedMesh);
		}
	}
}

void ABag::SpawnSmallItems(float DeltaTime)
{
	if (CarrotPrefab)
	{
		FNavLocation location{};
		auto origin = GetActorLocation();
		auto* navSystem = UNavigationSystemV1::GetCurrent(GetWorld());

		if (SmallItemSpawnTimer <= 0)
		{
			if (navSystem && navSystem->GetRandomPointInNavigableRadius(origin, 2000.0f, location))
			{
				auto* carrot = GetWorld()->SpawnActor(CarrotPrefab);
				carrot->SetActorLocation({location.Location.X, location.Location.Y, location.Location.Z + 100});
			}

			SmallItemSpawnTimer = SmallItemSpawnInterval;
		}
		else
		{
			SmallItemSpawnTimer -= DeltaTime;
		}
	}
}

void ABag::HandleBehaviorBasedOnWeight(float DeltaTime)
{
	if (Player)
	{
		switch(GetMovementState())
		{
		case MOVEMENTSTATE::FLYING:
			{
				Mesh->SetCanEverAffectNavigation(false);
				Mesh->SetSimulatePhysics(false);
				Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				float z = FMath::Clamp(FMath::Sin(GetWorld()->GetTimeSeconds()) * 100.0f  + 50.0f * GetWeight(), 50.0f, 99999.0f);
				SetActorLocation(UKismetMathLibrary::VLerp(GetActorLocation(), Player->GetActorLocation() + Player->GetActorUpVector() * z  - Player->GetActorRightVector() * 100.0f - Player->GetActorForwardVector() * 100.0f, DeltaTime));
				break;
			}
		case MOVEMENTSTATE::DRAGGING:
			{
				Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
				Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
				Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
				Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				Mesh->SetSimulatePhysics(true);
				Mesh->SetCanEverAffectNavigation(true);
				Mesh->SetLinearDamping(1.0f);
				Mesh->SetAngularDamping(100.0f);
				Mesh->GetBodyInstance()->SetMassScale(FMath::Clamp((GetWeight()), 0, 99999));
				FVector targetLocation = GetActorLocation();
				targetLocation.X = (Player->GetActorLocation()- Player->GetActorRightVector() * 100.0f - Player->GetActorForwardVector() * 100.0f + 50.0f * GetWeight()).X;
				targetLocation.Y = (Player->GetActorLocation()- Player->GetActorRightVector() * 100.0f - Player->GetActorForwardVector() * 100.0f + 50.0f * GetWeight()).Y;
				SetActorLocation(UKismetMathLibrary::VLerp(GetActorLocation(), targetLocation, DeltaTime / 2));
				break;
			}
		case MOVEMENTSTATE::FROZEN_GROUND:
			{
				Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
				Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
				Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
				Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				Mesh->SetSimulatePhysics(true);
				Mesh->SetCanEverAffectNavigation(true);
				Mesh->SetLinearDamping(100.0f);
				Mesh->SetAngularDamping(100.0f);
				Mesh->GetBodyInstance()->SetMassScale(FMath::Clamp((GetWeight()) * 99999, 0, 99999));
				break;
			}
		case MOVEMENTSTATE::FROZEN:
			{
				Mesh->SetCanEverAffectNavigation(false);
				Mesh->SetSimulatePhysics(false);
				Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				break;
			}
		}
	}
}

int ABag::GetWeight()
{
	return Player->PlayerInventory->GetWeight() - Player->EntityComponent->Properties.CarryWeightCurrentLevel;
}

MOVEMENTSTATE ABag::GetMovementState()
{
	int weight = GetWeight();
	if (weight >= StoppingThreshold)
	{
		return MOVEMENTSTATE::FROZEN_GROUND;
	}
	else if (weight >= WeightThreshold && IsBiengPulled)
	{
		return MOVEMENTSTATE::DRAGGING;
	}
	else if (weight >= WeightThreshold && !IsBiengPulled)
	{
		return MOVEMENTSTATE::FROZEN_GROUND;
	}
	else if (IsBiengPulled)
	{
		return MOVEMENTSTATE::FLYING;
	}
	else
	{
		return MOVEMENTSTATE::FROZEN;
	}
}

