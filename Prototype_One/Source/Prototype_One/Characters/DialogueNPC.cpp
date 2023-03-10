#include "DialogueNPC.h"

#include "PrototypeEnemy.h"
#include "Prototype_OneCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype_One/Widgets/PlayerHUD.h"
#include "Prototype_One/Widgets/DialogueWidget.h"

ADialogueNPC::ADialogueNPC()
{
	PrimaryActorTick.bCanEverTick = true;
	ExclamationMark = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Exclamation Mark Mesh"));
	ExclamationMark->SetupAttachment(GetMesh());
	MoleWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Mole Widget"));
	MoleWidget->SetupAttachment(RootComponent);
}

void ADialogueNPC::BeginPlay()
{
	Super::BeginPlay();
	GetMesh()->SetRenderCustomDepth(true);
	
	startPos = ExclamationMark->GetRelativeLocation();
}

void ADialogueNPC::UpdateInteractionOutline()
{
	if (auto* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		auto distanceToPlayer = (player->GetActorLocation() - GetActorLocation()).Length();
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
}

void ADialogueNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateInteractionOutline();

	ExclamationMark->SetRelativeLocation(startPos + FVector::UpVector * FMath::Sin(GetGameTimeSinceCreation() * 10.0f) * 20.0f);
}

void ADialogueNPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ADialogueNPC::Interact()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetInputMode(FInputModeUIOnly{});
	if (auto* charatcer = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		MoleWidget->SetVisibility(false);
		charatcer->PlayerHud->DialogueWidget->ShowDialogueBox(true);
		charatcer->GetCharacterMovement()->Velocity = {};
		charatcer->GetCharacterMovement()->StopMovementImmediately();
		charatcer->GetCharacterMovement()->StopActiveMovement();
		ExclamationMark->SetVisibility(false);
		charatcer->IsShopping = true;

		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APrototypeEnemy::StaticClass(), FoundActors);
		for(auto* enemy : FoundActors)
		{
			enemy->Destroy();
		}
	}
}


