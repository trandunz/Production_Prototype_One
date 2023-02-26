
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

int32 UPlayerInventory::Pickup(FItemDetails NewItemDetails, int32 Amount)
{
	for (FInventorySlot Slot : Items)
	{
		// Check if already have similar item
		if(Slot.Info.Type == NewItemDetails.Type)
		{
			// Add to stack if enough room
			if (Slot.Amount + Amount <= NewItemDetails.MaximumStackSize)
			{
				Slot.Amount = Slot.Amount + Amount;				
			}
			else // Fill current stack and create a new stack if possible
			{
				if (Slot.Amount <= NewItemDetails.MaximumStackSize)
				{
					Amount -= NewItemDetails.MaximumStackSize - Slot.Amount;
					if (Items.Num() == MaximumSlots)
					{
						return Amount;
					}
					else
					{
						FInventorySlot NewSlot;
						NewSlot.Amount = Amount;
						NewSlot.Info = NewItemDetails;
						Items.Push(NewSlot);
						return 0;
					}
				}
			}
		} // If inventory full, return all items
		else if(Items.Num() == MaximumSlots)
		{
			return Amount;
		}
		else // Add items to new slot
		{
			if (Amount > NewItemDetails.MaximumStackSize)
			{
				
			}
		}
	}
	

	
	return 0;
}

void UPlayerInventory::CalculateWeight()
{
	CurrentWeight = 0.0f;
	for (FInventorySlot Slot : Items)
	{
		CurrentWeight += Slot.Amount * Slot.Info.Weight;
	}
}

