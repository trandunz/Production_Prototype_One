#include "Bag.h"

#include "NavigationSystem.h"
#include "Characters/Prototype_OneCharacter.h"
#include "Components/RPGEntityComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ABag::ABag()
{
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = Mesh;
}

void ABag::BeginPlay()
{
	Super::BeginPlay();

	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		Player = player;
	}
}

void ABag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateInteractionOutline();
	
	if (IsOpen && OpenMesh)
	{
		Mesh->SetStaticMesh(OpenMesh);
		SetActorScale3D(FVector{1,1,1} * FMath::Clamp(Player->ValuablesCount * 0.1f, 0.1f, 99999));
		
		if (SpawnTimer > 0 && Player->ValuablesCount > 0)
		{
			SpawnTimer -= DeltaTime;
		}
		else if (SpawnTimer <= 0)
		{
			UE_LOG(LogTemp, Log, TEXT("Enemy Spawned"));
			SpawnTimer = rand() % 3 + 1;
			if (RabbitPrefab)
			{
				FNavLocation location{};
				auto origin = GetActorLocation();
				auto* navSystem = UNavigationSystemV1::GetCurrent(GetWorld());
				if (navSystem && navSystem->GetRandomPointInNavigableRadius(origin, 2000.0f, location))
				{
					auto* rabbit = GetWorld()->SpawnActor(RabbitPrefab);
					rabbit->SetActorLocation({location.Location.X, location.Location.Y, location.Location.Z + 100});

				}
					
			}
		}
	}
	else if (ClosedMesh)
	{
		SetActorScale3D(FVector{1,1,1} * FMath::Clamp(Player->ValuablesCount * 10.0f, 10.0f, 99999));
		Mesh->SetStaticMesh(ClosedMesh);
	}
	

	if (Player)
	{
		if (Player->ValuablesCount >= StoppingThreshold)
		{
			Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
			Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
			Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Mesh->SetSimulatePhysics(false);
			Mesh->SetCanEverAffectNavigation(true);
			Mesh->GetBodyInstance()->SetMassScale(FMath::Clamp((Player->ValuablesCount - Player->EntityComponent->CarryWightCurrentLevel) * 100090, 0, 999999));
			
		}
		else if (Player->ValuablesCount >= WeightThreshold && IsBiengPulled)
		{
			Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
			Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
			Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Mesh->SetSimulatePhysics(true);
			Mesh->SetCanEverAffectNavigation(true);
			Mesh->SetLinearDamping(1.0f);
			Mesh->SetAngularDamping(100.0f);
			Mesh->GetBodyInstance()->SetMassScale(FMath::Clamp((Player->ValuablesCount - Player->EntityComponent->CarryWightCurrentLevel), 0, 99999));
			FVector targetLocation = GetActorLocation();
			targetLocation.X = (Player->GetActorLocation()- Player->GetActorRightVector() * 100.0f - Player->GetActorForwardVector() * 100.0f + 50.0f * Player->ValuablesCount).X;
			targetLocation.Y = (Player->GetActorLocation()- Player->GetActorRightVector() * 100.0f - Player->GetActorForwardVector() * 100.0f + 50.0f * Player->ValuablesCount).Y;
			SetActorLocation(UKismetMathLibrary::VLerp(GetActorLocation(), targetLocation, DeltaTime / 2));
		}
		else if (Player->ValuablesCount >= WeightThreshold && !IsBiengPulled)
		{
			Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
			Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
			Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Mesh->SetSimulatePhysics(true);
			Mesh->SetCanEverAffectNavigation(true);
			Mesh->SetLinearDamping(100.0f);
			Mesh->SetAngularDamping(100.0f);
			Mesh->GetBodyInstance()->SetMassScale(FMath::Clamp((Player->ValuablesCount - Player->EntityComponent->CarryWightCurrentLevel) * 99999, 0, 99999));
		}
		else
		{
			Mesh->SetCanEverAffectNavigation(false);
			Mesh->SetSimulatePhysics(false);
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			float z = (FMath::Sin(GetWorld()->GetTimeSeconds()) * 100.0f )+ 50.0f * Player->ValuablesCount;
			SetActorLocation(UKismetMathLibrary::VLerp(GetActorLocation(), Player->GetActorLocation() + Player->GetActorUpVector() * z  - Player->GetActorRightVector() * 100.0f - Player->GetActorForwardVector() * 100.0f, DeltaTime));
		}
	}
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

