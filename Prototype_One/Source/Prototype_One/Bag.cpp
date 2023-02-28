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

	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (CurrentWeight >= StoppingThreshold)
		{
			Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
			Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
			Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Mesh->SetSimulatePhysics(true);
			Mesh->GetBodyInstance()->SetMassScale(FMath::Clamp(CurrentWeight - player->EntityComponent->CarryWightCurrentLevel, 0, 99999));
		}
		else if (CurrentWeight >= WeightThreshold)
		{
			Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
			Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
			Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Mesh->SetSimulatePhysics(true);
			Mesh->GetBodyInstance()->SetMassScale(FMath::Clamp(CurrentWeight - player->EntityComponent->CarryWightCurrentLevel, 0, 99999));
			FVector targetLocation = GetActorLocation();
			targetLocation.X = (player->GetActorLocation()- player->GetActorRightVector() * 100.0f - player->GetActorForwardVector() * 100.0f).X;
			targetLocation.Y = (player->GetActorLocation()- player->GetActorRightVector() * 100.0f - player->GetActorForwardVector() * 100.0f).Y;
			SetActorLocation(UKismetMathLibrary::VLerp(GetActorLocation(), targetLocation, DeltaTime / 2));
		}
		else
		{
			Mesh->SetSimulatePhysics(false);
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SetActorLocation(UKismetMathLibrary::VLerp(GetActorLocation(), player->GetActorLocation() + player->GetActorUpVector() * FMath::Sin(GetWorld()->GetTimeSeconds()) * 100.0f - player->GetActorRightVector() * 100.0f - player->GetActorForwardVector() * 100.0f, DeltaTime));
		}
	}
}

