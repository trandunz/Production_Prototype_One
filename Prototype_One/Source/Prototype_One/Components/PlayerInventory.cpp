
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
	if (bIsShopping)
	{
		// Increase coins by value of one item in slot
		Coins += Items[SlotIndex].Info.Value;

		Items[SlotIndex].Amount -= 1;
		if (Items[SlotIndex].Amount == 0)
		{
			// Remove Slot from Items
			Items.RemoveAt(SlotIndex);

			// Broadcast to widget to update UI
			OnSlotModified.Broadcast(SlotIndex, 0);			
		}
		else
		{
			// Broadcast to widget to update UI
			OnSlotModified.Broadcast(SlotIndex, Items[SlotIndex].Amount);		
		}
	
		// Refresh weight and coins
		CalculateWeight();
		UpdateCoins();
	}
}

void UPlayerInventory::SellSlot(const int32 SlotIndex)
{
	if (bIsShopping)
	{
		// Increase coins by value of item in slot * amount
		Coins += Items[SlotIndex].Info.Value * Items[SlotIndex].Amount;

		// Remove Slot from Items
		Items.RemoveAt(SlotIndex);

		// Broadcast to widget to update UI
		OnSlotModified.Broadcast(SlotIndex, 0);
	
		// Refresh weight and coins
		CalculateWeight();
		UpdateCoins();
	}
}

void UPlayerInventory::SellAll()
{
	// Sell all the things
	while(!Items.IsEmpty())
	{
		SellSlot(0);
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

void UPlayerInventory::DropOneItem(const int32 SlotIndex)
{
	if(Items.IsValidIndex(SlotIndex))
	{
		Items[SlotIndex].Amount -= 1;
		CalculateWeight();
		
		if (Items[SlotIndex].Amount == 0)
		{
			// Remove Slot from Items
			Items.RemoveAt(SlotIndex);
			OnSlotModified.Broadcast(SlotIndex, 0);

		}
		else
		{
			OnSlotModified.Broadcast(SlotIndex, Items[SlotIndex].Amount);
		}
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

void UPlayerInventory::BagDropped()
{
	while(!Items.IsEmpty())
	{
		DroppedItems.Add(Items[0]);
		Drop(0);
	}
}

void UPlayerInventory::BagReturned()
{
	int32 SlotIdx = 0;
	while(DroppedItems.IsValidIndex(SlotIdx))
	{
		for (int i = 0; i < DroppedItems[SlotIdx].Amount; i++)
		{
			Pickup(DroppedItems[SlotIdx].Info);
		}

		SlotIdx++;
	}

	for (int i = 0; i < Items.Num(); i++)
	{
		// Broadcast to Inventory widget
		OnSlotModified.Broadcast(i, Items[i].Amount);
	}
	
	DroppedItems.Empty();
}

void UPlayerInventory::BagLost()
{
	DroppedItems.Empty();
}

void UPlayerInventory::SetIsShopping(bool IsShopping)
{
	bIsShopping = IsShopping;

	// Tell widget if shopping to change some text (Drop Item/Sell Stack)
	OnIsShopping.Broadcast(IsShopping);
}

bool UPlayerInventory::UsePotion()
{
	// Find a potion in inventory, return true if successful and remove it
	bool bHasPotion = false;
	for (int SlotIdx = 0; SlotIdx < Items.Num(); SlotIdx++)
	{
		if (Items[SlotIdx].Info.Type == EItemType::Potion)
		{
			if (Items[SlotIdx].Amount > 1)
			{
				Items[SlotIdx].Amount -= 1;
				OnSlotModified.Broadcast(SlotIdx, Items[SlotIdx].Amount);
				bHasPotion = true;
			}
			else
			{
				Drop(SlotIdx);
				bHasPotion = true;
			}
			
			break;
		}
	}
	return bHasPotion;
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

