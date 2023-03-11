// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "Prototype_One/Components/PlayerInventory.h"
#include "Prototype_One/Characters/Prototype_OneCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

void UInventoryWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (auto* player = Cast<APrototype_OneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		FText Rabbits = FText::AsCultureInvariant(FString::FromInt(player->PlayerInventory->GetRabbitSpawnCount()));
		FText Masked = FText::AsCultureInvariant(FString::FromInt(player->PlayerInventory->GetMaskedSpawnCount()));
		FText Kings = FText::AsCultureInvariant(FString::FromInt(player->PlayerInventory->GetKingSpawnCount()));
	
	
		RabbitCount->SetText(Rabbits);
		MaskedCount->SetText(Masked);
		KingCount->SetText(Kings);
	}
}
