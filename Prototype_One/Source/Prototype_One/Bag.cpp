#include "Bag.h"

#include "Item.h"
#include "NavigationSystem.h"
#include "Characters/Prototype_OneCharacter.h"
#include "Components/RPGEntityComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Rope.h"
#include "CableComponent/Classes/CableComponent.h"
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

	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		Player = player;
	}

	if (RopePrefab)
	{
		Rope = Cast<ARope>(GetWorld()->SpawnActor(RopePrefab));
		Rope->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
		Rope->SetActorScale3D({0.1f,0.1f,0.1f});
		TArray<UCableComponent*> cableComponents{};
		Rope->GetComponents<UCableComponent>(cableComponents);
		
		if (cableComponents.Num() > 0)
		{
			CableComponent = cableComponents[0];
		}

		if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
		{
			CableComponent->SetAttachEndToComponent(player->GetMesh());
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
	if (IsOpen)
	{
		int weight = (CarrotCount + MeatCount + AntlerCount + MaskCount + CrownCount) - Player->EntityComponent->Properties.CarryWeightCurrentLevel;
	
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItem::StaticClass(), actors);
		TArray<AActor*> îtemsInRange;
		for(auto* actor : actors)
		{
			if ((actor->GetActorLocation() - GetActorLocation()).Length() <= SuctionRadius * FMath::Clamp((weight), 1, 99999))
			{
				îtemsInRange.Add(actor);
			}
		}

		for(auto* actor : îtemsInRange)
		{
			if (auto* item = Cast<AItem>(actor))
			{
				if (item->IsPickupable)
				{
					if ((actor->GetActorLocation() - GetActorLocation()).Length() <= 40.0f)
					{
			
						switch(item->ItemDetails.Type)
						{
						case EItemType::Meat:
							{
								MeatCount++;
								break;
							}
						case EItemType::Antler:
							{
								AntlerCount++;
								break;
							}
						case EItemType::Mask:
							{
								MaskCount++;
								break;
							}
						case EItemType::Crown:
							{
								CrownCount++;
								break;
							}
						case EItemType::Carrot:
							{
								CarrotCount++;
								break;
							}
						default:
							{
								break;
							}
						}
						actor->Destroy();
					}
					else
					{
						item->Mesh->SetSimulatePhysics(false);
						item->Mesh->SetCollisionProfileName("Trigger");
						actor->SetActorLocation(FMath::Lerp<FVector>(actor->GetActorLocation(), GetActorLocation(), DeltaTime * 10.0f));
					}
				}
			}
		}
	}
	
}

void ABag::SpawnEnemies(float DeltaTime)
{
	int weight = (CarrotCount + MeatCount + AntlerCount + MaskCount + CrownCount) - Player->EntityComponent->Properties.CarryWeightCurrentLevel;
	if (IsOpen && OpenMesh && RabbitPrefab && MaskedPrefab && KingPrefab)
	{
		Mesh->SetStaticMesh(OpenMesh);
		SetActorScale3D(FVector{1,1,1} * FMath::Clamp(weight * 0.1f, 0.1f, 99999));
		
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
			if (CarrotCount > 10 || MeatCount > 10)
			{
				if (navSystem && navSystem->GetRandomPointInNavigableRadius(origin, 2000.0f, location))
				{
					auto* rabbit = GetWorld()->SpawnActor(RabbitPrefab);
					rabbit->SetActorLocation({location.Location.X, location.Location.Y, location.Location.Z + 100});
				}
			}
			if (AntlerCount > 10)
			{
				if (navSystem && navSystem->GetRandomPointInNavigableRadius(origin, 2000.0f, location))
				{
					auto* masked = GetWorld()->SpawnActor(MaskedPrefab);
					masked->SetActorLocation({location.Location.X, location.Location.Y, location.Location.Z + 100});
				}
			}
			if (MaskCount > 10)
			{
				if (navSystem && navSystem->GetRandomPointInNavigableRadius(origin, 2000.0f, location))
				{
					auto* king = GetWorld()->SpawnActor(KingPrefab);
					king->SetActorLocation({location.Location.X, location.Location.Y, location.Location.Z + 100});
				}
			}
		}
	}
	else if (ClosedMesh)
	{
		SetActorScale3D(FVector{1,1,1} * FMath::Clamp(weight * 10.0f, 10.0f, 99999));
		Mesh->SetStaticMesh(ClosedMesh);
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
	int weight = (CarrotCount + MeatCount + AntlerCount + MaskCount + CrownCount) - Player->EntityComponent->Properties.CarryWeightCurrentLevel;
	if (Player)
	{
		if (weight >= StoppingThreshold)
		{
			Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
			Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
			Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Mesh->SetSimulatePhysics(false);
			Mesh->SetCanEverAffectNavigation(true);
			Mesh->GetBodyInstance()->SetMassScale(FMath::Clamp((weight) * 100090, 0, 999999));
			
		}
		else if (weight >= WeightThreshold && IsBiengPulled)
		{
			Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
			Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
			Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Mesh->SetSimulatePhysics(true);
			Mesh->SetCanEverAffectNavigation(true);
			Mesh->SetLinearDamping(1.0f);
			Mesh->SetAngularDamping(100.0f);
			Mesh->GetBodyInstance()->SetMassScale(FMath::Clamp((weight), 0, 99999));
			FVector targetLocation = GetActorLocation();
			targetLocation.X = (Player->GetActorLocation()- Player->GetActorRightVector() * 100.0f - Player->GetActorForwardVector() * 100.0f + 50.0f * weight).X;
			targetLocation.Y = (Player->GetActorLocation()- Player->GetActorRightVector() * 100.0f - Player->GetActorForwardVector() * 100.0f + 50.0f * weight).Y;
			SetActorLocation(UKismetMathLibrary::VLerp(GetActorLocation(), targetLocation, DeltaTime / 2));
		}
		else if (weight >= WeightThreshold && !IsBiengPulled)
		{
			Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
			Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
			Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Mesh->SetSimulatePhysics(true);
			Mesh->SetCanEverAffectNavigation(true);
			Mesh->SetLinearDamping(100.0f);
			Mesh->SetAngularDamping(100.0f);
			Mesh->GetBodyInstance()->SetMassScale(FMath::Clamp((weight) * 99999, 0, 99999));
		}
		else
		{
			Mesh->SetCanEverAffectNavigation(false);
			Mesh->SetSimulatePhysics(false);
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			float z = FMath::Clamp(FMath::Sin(GetWorld()->GetTimeSeconds()) * 100.0f  + 50.0f * weight, 50.0f, 99999.0f);
			SetActorLocation(UKismetMathLibrary::VLerp(GetActorLocation(), Player->GetActorLocation() + Player->GetActorUpVector() * z  - Player->GetActorRightVector() * 100.0f - Player->GetActorForwardVector() * 100.0f, DeltaTime));
		}
	}
}

