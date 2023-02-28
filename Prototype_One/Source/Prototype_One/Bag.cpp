#include "Bag.h"

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
}

void ABag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateInteractionOutline();
	
	if (IsOpen && OpenMesh)
	{
		Mesh->SetStaticMesh(OpenMesh);
	}
	else if (ClosedMesh)
	{
		Mesh->SetStaticMesh(ClosedMesh);
	}

	
	SetActorScale3D(FVector{1,1,1} * FMath::Clamp(0.25f * CurrentWeight, 0.5f, 99999));

	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (CurrentWeight >= StoppingThreshold)
		{
			Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
			Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
			Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Mesh->SetSimulatePhysics(false);
			Mesh->GetBodyInstance()->SetMassScale(FMath::Clamp((CurrentWeight - player->EntityComponent->CarryWightCurrentLevel) * 100090, 0, 999999));
			
		}
		else if (CurrentWeight >= WeightThreshold)
		{
			Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
			Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
			Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Mesh->SetSimulatePhysics(true);
			Mesh->GetBodyInstance()->SetMassScale(FMath::Clamp((CurrentWeight - player->EntityComponent->CarryWightCurrentLevel), 0, 99999));
			FVector targetLocation = GetActorLocation();
			targetLocation.X = (player->GetActorLocation()- player->GetActorRightVector() * 100.0f - player->GetActorForwardVector() * 100.0f + 50.0f * CurrentWeight).X;
			targetLocation.Y = (player->GetActorLocation()- player->GetActorRightVector() * 100.0f - player->GetActorForwardVector() * 100.0f + 50.0f * CurrentWeight).Y;
			SetActorLocation(UKismetMathLibrary::VLerp(GetActorLocation(), targetLocation, DeltaTime / 2));
		}
		else
		{
			Mesh->SetSimulatePhysics(false);
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			float z = (FMath::Sin(GetWorld()->GetTimeSeconds()) * 100.0f )+ 50.0f * CurrentWeight;
			SetActorLocation(UKismetMathLibrary::VLerp(GetActorLocation(), player->GetActorLocation() + player->GetActorUpVector() * z  - player->GetActorRightVector() * 100.0f - player->GetActorForwardVector() * 100.0f, DeltaTime));
		}
	}
}

void ABag::Interact()
{
	IsOpen = !IsOpen;
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

