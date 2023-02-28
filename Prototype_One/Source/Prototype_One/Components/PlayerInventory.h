
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prototype_One/Item.h"
#include "PlayerInventory.generated.h"

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemDetails Info;

	
	// Static function used to sort Items array
	static bool CompareAscending(const FInventorySlot& InA, const FInventorySlot& InB)
	{
		return InA.Info.Type < InB.Info.Type;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOTYPE_ONE_API UPlayerInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerInventory();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* Attempts to pickup an amount of items, returns amount not added if there wasn't enough
	 * space in inventory */
	void Pickup(FItemDetails PickedUpItemInfo);

	/* Sells the slot passed in */
	void Sell(FInventorySlot Slot);

	/* Sorts the Items in the inventory by type */
	void SortByType(FInventorySlot Slot);

	void Drop(FInventorySlot Slot);

	TArray<FInventorySlot> GetItems() { return Items; }
private:
	/* Sum all items held in inventory */
	void CalculateWeight();

public:
	TArray<FInventorySlot> Items;
	float MaximumWeight;
	float CurrentWeight;
	int32 Coins;
};
