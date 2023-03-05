
#include "PlayerInventory.h"

#include <string>

#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UPlayerInventory::UPlayerInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlayerInventory::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPlayerInventory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPlayerInventory::Sell(const int32 SlotIndex)
{
	// Increase coins by value of item in slot * amount
	Coins += Items[SlotIndex].Info.Value * Items[SlotIndex].Amount;

	// Remove Slot from Items
	Items.RemoveAt(SlotIndex);

	// Broadcast to widget to update UI
	OnSlotModified.Broadcast(SlotIndex, 0);
	
	// Refresh weight
	CalculateWeight();
}

void UPlayerInventory::SellAll()
{
	// Sell all the things
	while(!Items.IsEmpty())
	{
		Sell(0);
	}
}

void UPlayerInventory::SortByType()
{
	Algo::Sort(Items, [](const FInventorySlot& InA, const FInventorySlot& InB)
	{
		return FInventorySlot::CompareAscending(InA, InB);
	}
	);
}

void UPlayerInventory::Drop(const int32 SlotIndex)
{
	if(Items.IsValidIndex(SlotIndex))
	{
		Items.RemoveAt(SlotIndex);
		CalculateWeight();

		OnSlotModified.Broadcast(SlotIndex, 0);
	}
}

void UPlayerInventory::Pickup(FItemDetails PickedUpItemInfo)
{
	if (Items.IsEmpty())
	{
		AddNewSlot(PickedUpItemInfo);
		return;
	}
	
	// Try add item to not full stack
	for (int SlotIdx = 0; SlotIdx < Items.Num(); SlotIdx++)
	{
		if (Items[SlotIdx].Info.Type == PickedUpItemInfo.Type)
		{
			// Add to stack if enough room
			if (Items[SlotIdx].Amount < PickedUpItemInfo.MaximumStackSize)
			{
				Items[SlotIdx].Amount += 1;
				
				// Refresh the weight of the Inventory
				CalculateWeight();

				// Broadcast to Inventory widget
				OnSlotModified.Broadcast(SlotIdx, Items[SlotIdx].Amount);
				return;
			}
		}
	}
	
	AddNewSlot(PickedUpItemInfo);
}

void UPlayerInventory::SubtractCoins(int32 AmountToSubtract)
{
	Coins -= AmountToSubtract;
	UpdateCoins();
}

void UPlayerInventory::UpdateCoins()
{
	OnUpdateCoins.Broadcast();
}

int32 UPlayerInventory::GetRabbitSpawnCount()
{
	int32 RabbitsToSpawn = 0;
	for (FInventorySlot Slot : Items)
	{
		if (Slot.Info.Type == EItemType::Carrot)
		{
			if (Slot.Amount == Slot.Info.MaximumStackSize)
			{
				RabbitsToSpawn += 1;
			}
		}
	}
	return RabbitsToSpawn;
}

int32 UPlayerInventory::GetMaskedSpawnCount()
{
	int32 MaskedToSpawn = 0;
	for (FInventorySlot Slot : Items)
	{
		if (Slot.Info.Type == EItemType::Antler)
		{
			if (Slot.Amount == Slot.Info.MaximumStackSize)
			{
				MaskedToSpawn += 1;
			}
		}
	}
	return MaskedToSpawn;
}

int32 UPlayerInventory::GetKingSpawnCount()
{
	int32 KingToSpawn = 0;
	for (FInventorySlot Slot : Items)
	{
		if (Slot.Info.Type == EItemType::Mask)
		{
			if (Slot.Amount == Slot.Info.MaximumStackSize)
			{
				KingToSpawn += 1;
			}
		}
	}
	return KingToSpawn;
}

void UPlayerInventory::PrintInventory()
{
	for (FInventorySlot Slot : Items)
	{
		UKismetSystemLibrary::PrintString(GetWorld(),FString::FromInt(Slot.Amount), true, true);
	}
}

void UPlayerInventory::CalculateWeight()
{
	CurrentWeight = 0.0f;
	for (const FInventorySlot Slot : Items)
	{
		CurrentWeight += Slot.Amount * Slot.Info.Weight;
	}
}

void UPlayerInventory::AddNewSlot(FItemDetails ItemInfoToAdd)
{
	// Add items to new slot					
	FInventorySlot NewSlot;
	NewSlot.Amount = 1;
	NewSlot.Info = ItemInfoToAdd;
	int32 NewSlotIndex = Items.Add(NewSlot);

	// Refresh the weight of the Inventory
	CalculateWeight();

	// Broadcast to Inventory widget
	OnNewSlot.Broadcast(NewSlotIndex, NewSlot);
}

