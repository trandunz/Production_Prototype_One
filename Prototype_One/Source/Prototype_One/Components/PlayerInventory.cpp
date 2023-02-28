
#include "PlayerInventory.h"

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
	
	for (FInventorySlot& Slot : Items)
	{
		// Check if already have similar item
		if(Slot.Info.Type == PickedUpItemInfo.Type)
		{
			// Add to stack if enough room
			if (Slot.Amount < PickedUpItemInfo.MaximumStackSize)
			{
				Slot.Amount += 1;	
			}
			else // Add items to new slot
			{
				FInventorySlot NewSlot;
				NewSlot.Amount = 1;
				NewSlot.Info = PickedUpItemInfo;
				Items.Add(NewSlot);
			}
		} 
		else // Add items to new slot
		{
			FInventorySlot NewSlot;
			NewSlot.Amount = 1;
			NewSlot.Info = PickedUpItemInfo;
			Items.Add(NewSlot);
		}
	}

	// Refresh the weight of the Inventory
	CalculateWeight();
}

void UPlayerInventory::CalculateWeight()
{
	CurrentWeight = 0.0f;
	for (const FInventorySlot Slot : Items)
	{
		CurrentWeight += Slot.Amount * Slot.Info.Weight;
	}
}

