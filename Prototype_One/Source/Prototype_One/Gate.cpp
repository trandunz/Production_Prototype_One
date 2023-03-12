#include "Gate.h"

#include "Controllers/PrototypePlayerController.h"
#include "Prototype_One/Characters/Prototype_OneCharacter.h"
#include "Kismet/GameplayStatics.h"

AGate::AGate()
{
	PrimaryActorTick.bCanEverTick = true;

	ArchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArchMesh"));
	RootComponent = ArchMesh;
	GateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMesh"));
	GateMesh->SetupAttachment(ArchMesh);
}

void AGate::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGate::UpdateInteractionOutline()
{
	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		auto distanceToPlayer = (player->GetActorLocation() - GetActorLocation()).Length();
		if (distanceToPlayer <= 300 && player->CanWinGame)
		{
			ArchMesh->SetRenderCustomDepth(true);
			ArchMesh->CustomDepthStencilValue = 1;
			GateMesh->SetRenderCustomDepth(true);
			GateMesh->CustomDepthStencilValue = 1;
		}
		else
		{
			ArchMesh->SetRenderCustomDepth(false);
			ArchMesh->CustomDepthStencilValue = 1;
			GateMesh->SetRenderCustomDepth(false);
			GateMesh->CustomDepthStencilValue = 1;
		}
	}
}

void AGate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateInteractionOutline();
}

void AGate::Interact()
{
	if (auto* player = Cast<APrototypePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		if (auto* character = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
		{
			if (character->CanWinGame)
				player->SwitchLevel("Level_WinScreen");
		}
	}
	
}

