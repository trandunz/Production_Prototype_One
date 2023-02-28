#include "Bag.h"

#include "Characters/Prototype_OneCharacter.h"
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
		SetActorLocation(UKismetMathLibrary::VLerp(GetActorLocation(), player->GetActorLocation() + player->GetActorUpVector() * FMath::Sin(GetWorld()->GetTimeSeconds()) * 100.0f - player->GetActorRightVector() * 100.0f - player->GetActorForwardVector() * 100.0f, DeltaTime));
	}
}

