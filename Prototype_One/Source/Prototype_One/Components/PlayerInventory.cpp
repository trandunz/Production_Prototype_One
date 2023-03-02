
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

	// Refresh weight
	CalculateWeight();
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
	Items.RemoveAt(SlotIndex);
	CalculateWeight();
}

void UPlayerInventory::Pickup(FItemDetails PickedUpItemInfo)
{
	if (Items.IsEmpty())
	{
		AddNewSlot(PickedUpItemInfo);
		return;
	}
	
	// Try add item to incomplete stack
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
				return;
			}
		}
	}
	
	AddNewSlot(PickedUpItemInfo);
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
	Items.Add(NewSlot);
	
	// Refresh the weight of the Inventory
	CalculateWeight();
}

