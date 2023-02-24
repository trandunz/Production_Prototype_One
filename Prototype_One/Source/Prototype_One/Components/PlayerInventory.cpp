
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

bool UPlayerInventory::Pickup(FItemDetails NewItemDetails, int32 Amount)
{
	for (FInventorySlot Slot : Items)
	{
		if(Slot.Info.Type == NewItemDetails.Type)
		{
			if (Slot.Amount + Amount <= NewItemDetails.MaximumStackSize)
			{
				Slot.Amount = Slot.Amount + Amount;				
			}
		}
	}
	
	if (Items.Num() == MaximumSlots)
	{
		return false;
	}
	
	return true;
}

void UPlayerInventory::CalculateWeight()
{
	CurrentWeight = 0.0f;
	for (FInventorySlot Slot : Items)
	{
		CurrentWeight += Slot.Amount * Slot.Info.Weight;
	}
}

