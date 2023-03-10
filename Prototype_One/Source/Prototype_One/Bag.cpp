#include "Bag.h"

#include "Item.h"
#include "NavigationSystem.h"
#include "Characters/Prototype_OneCharacter.h"
#include "Characters/DialogueNPC.h"
#include "Components/RPGEntityComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Rope.h"
#include "CableComponent/Classes/CableComponent.h"
#include "Components/PlayerInventory.h"
#include "Prototype_One/Item.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

ABag::ABag()
{
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = Mesh;

	WingRightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Right Wing"));
	WingRightMesh->SetupAttachment(RootComponent);
	WingLeftMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Left Wing"));
	WingLeftMesh->SetupAttachment(RootComponent);
	WingLeftMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WingRightMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//CableComponent->SetAttachEndTo(this, FName(Mesh->GetName()));

	//Constraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("Rope Constraint"));
	//Constraint->SetupAttachment(RootComponent);
	//Constraint->ConstraintActor2 = this;
}

void ABag::BeginPlay()
{
	Super::BeginPlay();

	if (RopePrefab)
	{
		Rope = Cast<ARope>(GetWorld()->SpawnActor(RopePrefab));
		Rope->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		TArray<UCableComponent*> cableComponents{};
		Rope->GetComponents<UCableComponent>(cableComponents);
		
		if (cableComponents.Num() > 0)
		{
			CableComponent = cableComponents[0];
		}

		if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
		{
			Player = player;
			CableComponent->AttachToComponent(Player->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("RopeSocket"));
			CableComponent->SetAttachEndTo(this, FName(""));
			CableComponent->EndLocation = {};
			//Constraint->ConstraintActor1 = Player;
		}
	}

	SetActorScale3D(FVector{1,1,1} * FMath::Clamp((((float)GetWeight() / (float)StoppingThreshold)) * 2 + 1.0f, 1.0f, 5.0f));
	Mesh->SetStaticMesh(ClosedMesh);
	
}

void ABag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	auto velocity = GetActorLocation() - prevPos;
	SetActorRotation(FRotator{GetActorRotation().Pitch, UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetActorLocation() + velocity * 10.0f).Yaw - 90.0f, GetActorRotation().Roll});
	
	UpdateInteractionOutline();
	AttractItems(DeltaTime);
	SpawnEnemies(DeltaTime);
	SpawnSmallItems(DeltaTime);
	HandleBehaviorBasedOnWeight(DeltaTime);
	FlapWings();

	

	if (Rope)
		Rope->SetActorScale3D({1.0f,1.0f,1.0f});
}

void ABag::Interact()
{
	//IsBiengPulled = !IsBiengPulled;

	if (IsBiengPulled == true)
	{
		IsBiengPulled = false;
		//if (Constraint)
		//	Constraint->ConstraintActor1 = nullptr;
		
		if (CableComponent)
			CableComponent->bAttachEnd = false;
	}
	else if (IsBiengPulled == false && IsWithinInteractionRange == true)
	{
		IsBiengPulled = true;

		//if (Constraint)
		//	Constraint->ConstraintActor1 = Player;
//
		if (CableComponent)
			CableComponent->bAttachEnd = true;
		
		if (Player)
		{
			if (Player->HasRespawnedOnce == true)
			{
				Player->PlayerInventory->BagReturned();
				Player->HasRespawnedOnce = false;
			}
		}
	}
}

void ABag::UpdateInteractionOutline()
{
	if (auto* character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		auto distanceToPlayer = (character->GetActorLocation() - GetActorLocation()).Length();
		if (distanceToPlayer <= InteractionRange)
		{
			IsWithinInteractionRange = true;
			Mesh->SetRenderCustomDepth(true);
			Mesh->CustomDepthStencilValue = 1;
		}
		else
		{
			IsWithinInteractionRange = false;
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
			if ((actor->GetActorLocation() - GetActorLocation()).Length() <= (SuctionRadius * (float)weight / (float)StoppingThreshold) + 500.0f)
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
						ItemCast->SetActorLocation(FMath::Lerp<FVector>(ItemCast->GetActorLocation(), GetActorLocation() + (GetActorLocation()- prevPos) , (DeltaTime * 2.0f) + (DeltaTime * (GetActorLocation()- prevPos).Length() * 3.0f)));
						
					}
				}
			}
		}
	}
	prevPos = GetActorLocation();
}

void ABag::SpawnEnemies(float DeltaTime)
{
	if (Player)
	{
		int weight = GetWeight();
		if (IsOpen && OpenMesh && RabbitPrefab && MaskedPrefab && KingPrefab)
		{
			Mesh->SetStaticMesh(OpenMesh);
			
			SetActorScale3D(FVector{1,1,1} * FMath::Clamp((((float)weight / (float)StoppingThreshold)) * 2 + 1.0f, 1.0f, 5.0f));
			
			TArray<AActor*> actors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADialogueNPC::StaticClass(), actors);
			for(auto npcActor : actors)
			{
				if (auto* npc = Cast<ADialogueNPC>(npcActor))
				{
					if ((npc->GetActorLocation() - GetActorLocation()).Length() >= DistanceFromNPC)
					{
						if (EnemySpawnTimer > 0)
						{
							EnemySpawnTimer -= DeltaTime;
						}
						else if (EnemySpawnTimer <= 0 && weight > 0)
						{
							UE_LOG(LogTemp, Log, TEXT("Enemy Spawned"));
							EnemySpawnTimer = EnemySpawnTimerLength;
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
										FVector pos = {location.Location.X, location.Location.Y, location.Location.Z + 700};
										FRotator rot = {0,0,0};
										FActorSpawnParameters spawnParams{};
										auto* king = GetWorld()->SpawnActor(KingPrefab, &pos, &rot);
									}
								}
							}
						}
					}
				}
			}
		}
		else if (ClosedMesh)
		{
			SetActorScale3D(FVector{1,1,1} * FMath::Clamp((((float)weight / (float)StoppingThreshold)) * 2 + 1.0f, 1.0f, 5.0f));
			Mesh->SetStaticMesh(ClosedMesh);
		}
	}
}
		
void ABag::SpawnSmallItems(float DeltaTime)
{
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADialogueNPC::StaticClass(), actors);

	for(auto npcActor : actors)
	{
		if (auto* npc = Cast<ADialogueNPC>(npcActor))
		{
			if ((npc->GetActorLocation() - GetActorLocation()).Length() >= DistanceFromNPC)
			{
				if (CarrotPrefab && PebblePrefab && StickPrefab)
				{
					FNavLocation location{};
					auto origin = GetActorLocation();
					auto* navSystem = UNavigationSystemV1::GetCurrent(GetWorld());
					auto* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
					if (SmallItemSpawnTimer <= 0)
					{
						if (navSystem && navSystem->GetRandomPointInNavigableRadius(origin + player->GetActorForwardVector() * 2000.0f, SpawnRadius, location))
						{
							UE_LOG(LogTemp, Warning, TEXT("Spawned small item"));
							
							// Pick a random object between carrot pebble and stick
							int32 RandomNumber = UKismetMathLibrary::RandomInteger(3);
							switch(RandomNumber)
							{
							case 0:
								{
									auto* SmallSpawnableItem = GetWorld()->SpawnActor(CarrotPrefab);
									SmallSpawnableItem->SetActorLocation({location.Location.X, location.Location.Y, location.Location.Z + 100});
									break;
								}
							case 1:
								{
									auto* SmallSpawnableItem = GetWorld()->SpawnActor(PebblePrefab);
									SmallSpawnableItem->SetActorLocation({location.Location.X, location.Location.Y, location.Location.Z + 100});
									break;
								}
							case 2:
								{
									auto* SmallSpawnableItem = GetWorld()->SpawnActor(StickPrefab);
									SmallSpawnableItem->SetActorLocation({location.Location.X, location.Location.Y, location.Location.Z + 100});
									break;
								}
							default:
								{
									auto* SmallSpawnableItem = GetWorld()->SpawnActor(CarrotPrefab);
									SmallSpawnableItem->SetActorLocation({location.Location.X, location.Location.Y, location.Location.Z + 100});
								}
							}
							// Debug random number output
							//UKismetSystemLibrary::PrintString(GetWorld(), FString::FromInt(RandomNumber));
						}
						SmallItemSpawnTimer = SmallItemSpawnInterval;
					}
					else
					{
						SmallItemSpawnTimer -= DeltaTime;
					}
				}
			}
			else
			{
				//UE_LOG(LogTemp, Warning, TEXT("Distance between bag & mole %f"), (npc->GetActorLocation() - GetActorLocation()).Length());
				
			}
		}
	}
	
	//if (CarrotPrefab && PebblePrefab && StickPrefab)
	//{
	//	FNavLocation location{};
	//	auto origin = GetActorLocation();
	//	auto* navSystem = UNavigationSystemV1::GetCurrent(GetWorld());
//
	//	if (SmallItemSpawnTimer <= 0)
	//	{
	//		if (navSystem && navSystem->GetRandomPointInNavigableRadius(origin, SpawnRadius, location))
	//		{
	//			// Pick a random object between carrot pebble and stick
	//			int32 RandomNumber = UKismetMathLibrary::RandomInteger(3);
	//			switch(RandomNumber)
	//			{
	//			case 0:
	//				{
	//					auto* SmallSpawnableItem = GetWorld()->SpawnActor(CarrotPrefab);
	//					SmallSpawnableItem->SetActorLocation({location.Location.X, location.Location.Y, location.Location.Z + 100});
	//					break;
	//				}
	//			case 1:
	//				{
	//					auto* SmallSpawnableItem = GetWorld()->SpawnActor(PebblePrefab);
	//					SmallSpawnableItem->SetActorLocation({location.Location.X, location.Location.Y, location.Location.Z + 100});
	//					break;
	//				}
	//			case 2:
	//				{
	//					auto* SmallSpawnableItem = GetWorld()->SpawnActor(StickPrefab);
	//					SmallSpawnableItem->SetActorLocation({location.Location.X, location.Location.Y, location.Location.Z + 100});
	//					break;
	//				}
	//			default:
	//				{
	//					auto* SmallSpawnableItem = GetWorld()->SpawnActor(CarrotPrefab);
	//					SmallSpawnableItem->SetActorLocation({location.Location.X, location.Location.Y, location.Location.Z + 100});
	//				}
	//			}
	//			// Debug random number output
	//			//UKismetSystemLibrary::PrintString(GetWorld(), FString::FromInt(RandomNumber));
	//		}
	//		SmallItemSpawnTimer = SmallItemSpawnInterval;
	//	}
	//	else
	//	{
	//		SmallItemSpawnTimer -= DeltaTime;
	//	}
	//}
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
				float z = FMath::Clamp(FMath::Sin(GetWorld()->GetTimeSeconds()) * 100.0f  + 5.0f, 25.0f, 99999.0f);
				SetActorLocation(UKismetMathLibrary::VLerp(GetActorLocation(), Player->GetActorLocation() + Player->GetActorUpVector() * z - Player->GetActorForwardVector() * 100.0f, DeltaTime * 2.0f));
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
				targetLocation.X = (Player->GetActorLocation()- Player->GetActorForwardVector() * 300.0f + 20.0f).X;
				targetLocation.Y = (Player->GetActorLocation()- Player->GetActorForwardVector() * 300.0f + 20.0f).Y;
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
	return Player->PlayerInventory->GetWeight();
}

void ABag::FlapWings()
{
	WingLeftMesh->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale);
	WingRightMesh->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale);
	FRotator currentRotation = WingLeftMesh->GetRelativeRotation();
	
	if ((float)GetWeight() < (float)PlayerWeightThreshold)
		currentRotation.Yaw = FMath::Sin(GetGameTimeSinceCreation() * (20.0f + FMath::Clamp(FMath::Lerp(0.0f, 1.0f, (float)GetWeight() / (float)PlayerWeightThreshold) * 50.0f, 0.0f, 50.0f))) * 30.0f;

	if ((float)GetWeight() >= (float)PlayerWeightThreshold)
		currentRotation.Yaw = FMath::Sin(GetGameTimeSinceCreation() * 2.0f) * 30.0f;
		
	WingLeftMesh->SetRelativeRotation(currentRotation);
	WingRightMesh->SetRelativeRotation(currentRotation);
}

MOVEMENTSTATE ABag::GetMovementState()
{
	int weight = GetWeight();
	if (weight >= StoppingThreshold)
	{
		return MOVEMENTSTATE::FROZEN_GROUND;
	}
	else if (weight >= PlayerWeightThreshold && IsBiengPulled)
	{
		return MOVEMENTSTATE::DRAGGING;
	}
	else if (weight >= PlayerWeightThreshold && !IsBiengPulled)
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

