#include "DialogueNPC.h"

#include "Prototype_OneCharacter.h"
#include "Kismet/GameplayStatics.h"

ADialogueNPC::ADialogueNPC()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADialogueNPC::BeginPlay()
{
	Super::BeginPlay();
	GetMesh()->SetRenderCustomDepth(true);
}

void ADialogueNPC::UpdateInteractionOutline()
{
	auto distanceToPlayer = (UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation() - GetActorLocation()).Length();
	if (distanceToPlayer <= InteractionRange)
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

void ADialogueNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateInteractionOutline();
}

void ADialogueNPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ADialogueNPC::Interact()
{
	
}

